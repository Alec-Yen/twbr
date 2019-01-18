// Updated balance program to work with c++98

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include "PiMotor.h"
#include "MPU6050.h"
#include "I2Cdev.h"
#include "timing.h"

using namespace std;

// global variables
bool PRINT = 1; // set to 1 to print out angles and PID terms
bool DEBUG = 0; // set to 1 to avoid running the motors (testing only angle)
int p1 = 18; //Left PWM (refers to BCM numbers "GPIO 18", not the physical pins)
int d1 = 23; //Left DIR
int p2 = 12; //Right PWM
int d2 = 16; //Right DIR
double targetAngle = 0;
double RAD_TO_DEG = 180.0/3.14159;
int MAX_MOTOR = 200; // max is 255

// for PID method
double Kp, Kd, Ki; // PID coefficients 
double accY, accZ, gyroX; // IMU measurements
double iTerm = 0;
double prevAngle = 0;
clock_t prev_t;

// for timing
int STD_LOOP_TIME = 10; // in milliseconds TODO: in forum, Kas made this 9 but that doesn't make sense? 
int lastLoopTime = STD_LOOP_TIME;
int lastLoopUsefulTime = STD_LOOP_TIME;
unsigned long loopStartTime;
double sampleTime = STD_LOOP_TIME/1000.0; // in seconds

// for IMU calibration
enum IMU_Index {ACC_Y,ACC_Z,GYRO_X};
int sensorZero[3] = {0,0,0}; // calibration
int sensorValue[3] = {0,0,0}; // averaged raw IMU measurements


// multithreading shared variables
pthread_mutex_t lock; // for thread safe code
bool break_condition = false;


// calculate motor PWM from PID equation
void PID (double& motorPower, int& direction)
{
	double err,gyroRate,changeInAngle,pTerm,dTerm;
	double accAngle, gyroAngle, currentAngle;
	
	// calculate the angle of inclination
	accAngle = (double) atan2(accY, accZ) * RAD_TO_DEG; // degrees
	gyroRate = gyroX; // degrees/second
	gyroAngle = gyroRate*sampleTime; // degrees
	currentAngle = 0.99*(prevAngle + gyroAngle) + 0.01*(accAngle); // complementary filter
	
	// PID calculations
	pthread_mutex_lock (&lock);
	err = currentAngle - targetAngle; // targetAngle is 0
	pthread_mutex_unlock (&lock);
	changeInAngle = currentAngle - prevAngle;
	pTerm = Kp*err;
	iTerm += Ki*err*sampleTime;
	dTerm = Kd*changeInAngle/sampleTime;

	motorPower = pTerm + iTerm + dTerm;
	prevAngle = currentAngle;

	// keep within max power
	if (motorPower > MAX_MOTOR) motorPower = MAX_MOTOR;
	else if (motorPower < -MAX_MOTOR ) motorPower = -MAX_MOTOR;

	// determine direction and get magnitude of power
	if (motorPower < 0) {
		direction = 0;
		motorPower *= -1;
	}
	else direction = 1;

	// print statements
	if (PRINT) printf("accAngle %3.2f\t gyroAngle %3.6f\t\t currentAngle %3.2f\t pTerm %3.2f\t dTerm %3.2f\t motorPower = %3.2f\n",accAngle,gyroAngle,currentAngle,pTerm,dTerm,motorPower);

	// debug for testing without running motors
	if (DEBUG) {
		motorPower = 0;
		return;
	}
}

// balance robot
void* Balance (void* robot_)
{
	double motorPower;
	int direction;
	TWBR* robot = (TWBR *)robot_;

	I2Cdev::initialize();
	MPU6050 mpu;
	mpu.initialize();

	delay(100);

	for (int n=0; n<50; n++) {
		//mpu.getMotion6 (&sensorTemp[ACC_X],&sensorTemp[ACC_Y],&sensorTemp[ACC_Z],&sensorTemp[GYRO_X],&sensorTemp[GYRO_Y],&sensorTemp[GYRO_Z]);
		sensorZero[ACC_Y] += mpu.getAccelerationY();
		sensorZero[ACC_Z] += mpu.getAccelerationZ();
		sensorZero[GYRO_X] += mpu.getRotationX();
	}
	for (int i=0; i<3; i++) sensorZero[i] /= 50;
	sensorZero[ACC_Z] -= 16384; 

	printf("sensorZero[ACC_Z]=%d\nsensorZero[ACC_Y]=%d\nsensorZero[GYRO_X]=%d\n",sensorZero[ACC_Z],sensorZero[ACC_Y],sensorZero[GYRO_X]);
	int AVERAGE_TIMES = 5; // number of values averaged together



	// balancing loop
	loopStartTime = millis();
	while(!break_condition) {

		// read IMU values TODO: see if this works
		for (int i=0; i<3; i++) sensorValue[i] = 0;
		for (int n=0; n<AVERAGE_TIMES; n++) sensorValue[ACC_Y] += mpu.getAccelerationY();
		for (int n=0; n<AVERAGE_TIMES; n++) sensorValue[ACC_Z] += mpu.getAccelerationZ();
		for (int n=0; n<AVERAGE_TIMES; n++) sensorValue[GYRO_X] += mpu.getRotationX();
		for (int i=0; i<3; i++) sensorValue[i] = sensorValue[i]/AVERAGE_TIMES - sensorZero[i];
	

		accY = sensorValue[ACC_Y]/16384.0;
		accZ = sensorValue[ACC_Z]/16384.0;
		gyroX = sensorValue[GYRO_X]/131.0;

		accY = mpu.getAccelerationY()/16384.0;
		accZ = mpu.getAccelerationZ()/16384.0;
		gyroX = mpu.getRotationX()/131.0;

		PID(motorPower,direction);

		pthread_mutex_lock (&lock);
		robot->writePWMSame(direction,motorPower);
		//robot->moveSame(direction,motorPower,sampleTime*1000); // third argument is in milliseconds
		pthread_mutex_unlock (&lock);

		// code to make sure loop executes with precise timing
		//printf("lastLoopUsefulTime=%10d lastLoopTime=%10d\n",lastLoopUsefulTime,lastLoopTime);
		lastLoopUsefulTime = millis()-loopStartTime;
		if(lastLoopUsefulTime<STD_LOOP_TIME)			delay(STD_LOOP_TIME-lastLoopUsefulTime);
		lastLoopTime = millis() - loopStartTime;
		loopStartTime = millis();

	}
	return NULL;
}

// stop robot upon entering 'q'
void* Stop (void* robot_)
{
	char q;
	TWBR* robot = (TWBR *)robot_;

	scanf("%c",&q);
	if (q == 'q') {
		break_condition = true;
		printf("Breaking out of loop\n");

		pthread_mutex_lock (&lock);
		robot->wait(100);
//		robot->stop(); // also calls gpioTerminate()
		robot->writePWMSame(1,0);
		pthread_mutex_unlock (&lock);
	}
	return NULL;
}

// main function
int main(int argc, char** argv)
{
	// check command line arguments
	if (argc != 6) {
		fprintf(stderr,"usage: sudo %s Kp Ki Kd PRINT DEBUG\n",argv[0]);
		fprintf(stderr,"\tsudo %s 60 .5 .5 0 0\n",argv[0]);
		return 1;
	}
	Kp = atof(argv[1]);
	Ki = atof(argv[2]);
	Kd = atof(argv[3]);
	PRINT = atoi(argv[4]);
	DEBUG = atoi(argv[5]);

	// initialize everything
	TWBR *robot = new TWBR(p1,d1,p2,d2);
	prev_t = clock();
	
	// multithreading
	pthread_t loop_thread, break_thread;

	pthread_mutex_init (&lock, NULL);
	pthread_create (&loop_thread, NULL, Balance, robot);
	pthread_create (&break_thread, NULL, Stop, robot);
	pthread_join (loop_thread, NULL);
	pthread_join (break_thread, NULL);
	printf("Threads joined successfully\n");
	robot->writePWMSame(1,0);

	return 0;
}
