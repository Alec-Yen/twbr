#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h> // for threading
#include "rc_twbr.hpp"
#include "MPU6050.h"
#include "I2Cdev.h"
#include "twbr.h"
#include "timing.h"

using namespace std;

// global variables
bool PRINT_ANGLE = 1; // set to 1 to print out angles and PID terms
bool DEBUG = 0; // set to 1 to avoid running the motors (testing only angle)
double targetAngle = 0;
double RAD_TO_DEG = 180.0/3.14159;
int MAX_MOTOR = 40; // out of 100, TODO: trying to figure out optimal value, maybe should just be 100

// for timing
double sampleTime = 0.01; // in seconds
int STD_LOOP_TIME = 10; // in milliseconds TODO: in forum, Kas made this 9 but that doesn't make sense? 
int lastLoopTime = STD_LOOP_TIME;
int lastLoopUsefulTime = STD_LOOP_TIME;
unsigned long loopStartTime;


// for IMU calibration
enum IMU_Index {ACC_Y,ACC_Z,GYRO_X};
int sensorZero[3] = {0,0,0}; // calibration
int sensorValue[3] = {0,0,0}; // averaged raw IMU measurements


// for PID method
double Kp, Kd, Ki;  // PID coefficients
double accY, accZ, gyroX; // converted IMU measurements
double iTerm = 0;
double prevAngle = 0;

// multithreading shared variables
pthread_mutex_t lock; // for thread safe code
bool break_condition = false;

// for printing
static int skip = 0;

/*************************************************************************************************************/




// calculate motor PWM from PID equation
void PID (double& motorPower)
{
	double err,gyroRate,pTerm,dTerm;
	double accAngle, gyroAngle, currentAngle;


	// calculate the angle of inclination
	accAngle = (double) atan2(accY, accZ) * RAD_TO_DEG; // degrees
	gyroRate = gyroX; // degrees/second
	gyroAngle = gyroRate*sampleTime; // degrees
	currentAngle = 0.99*(prevAngle + gyroAngle) + 0.01*(accAngle); // complementary filter TODO: understand this

	// PID calculations
	pthread_mutex_lock (&lock);
	err = currentAngle - targetAngle;
	pthread_mutex_unlock (&lock);


	pTerm = Kp*err;
	iTerm += Ki*err; // TODO: don't need to account for time if time is set
	dTerm = Kd*(currentAngle - prevAngle);
	motorPower = pTerm + iTerm + dTerm;
	prevAngle = currentAngle;

	// keep within max power
	if (motorPower > MAX_MOTOR) motorPower = MAX_MOTOR;
	else if (motorPower < -MAX_MOTOR ) motorPower = -MAX_MOTOR;


	// TODO: had to include to make it work
	motorPower *= -1;

	// print statements
	if (PRINT_ANGLE && skip++==5) {
		skip = 0;
		printf("accAngle %.2f\t gyroAngle %.6f\t\t currentAngle %.2f\t\t motorPower %.2f\n",accAngle,gyroAngle,currentAngle,motorPower);
	//	if (PRINT_ANGLE) printf("pTerm = %.2f\t iTerm = %.2f\t dTerm = %.2f\t motorPower = %.2f\n",pTerm,iTerm,dTerm,motorPower);
	}

	// debug for testing without running motors
	if (DEBUG) {
		motorPower = 0; //DEBUGGING: for testing without running motors
		return;
	}


}

// balance robot
void* Balance (void* robot_)
{
	double motorPower;
	RClaw* robot = (RClaw *)robot_;

	I2Cdev::initialize();
	MPU6050 mpu;
	mpu.initialize();

	delay(100);

	// calibrate sensor TODO: see if this works
	for (int n=0; n<50; n++) {
		//mpu.getMotion6 (&sensorTemp[ACC_X],&sensorTemp[ACC_Y],&sensorTemp[ACC_Z],&sensorTemp[GYRO_X],&sensorTemp[GYRO_Y],&sensorTemp[GYRO_Z]);
		sensorZero[ACC_Y] += mpu.getAccelerationY();
		sensorZero[ACC_Z] += mpu.getAccelerationZ();
		sensorZero[GYRO_X] += mpu.getRotationX();
	}
	for (int i=0; i<3; i++) sensorZero[i] /= 50;
	sensorZero[ACC_Z] -= 16384; //TODO: figure out what value to put here (Kas uses 102)

	printf("sensorZero[ACC_Z]=%d\nsensorZero[ACC_Y]=%d\nsensorZero[GYRO_X]=%d\n",sensorZero[ACC_Z],sensorZero[ACC_Y],sensorZero[GYRO_X]);




	// balancing loop
	loopStartTime = millis();
	while(!break_condition) {

		// read IMU values TODO: see if this works
		for (int i=0; i<3; i++) sensorValue[i] = 0;
		for (int n=0; n<5; n++) {
			sensorValue[ACC_Y] += mpu.getAccelerationY();
			sensorValue[ACC_Z] += mpu.getAccelerationZ();
			sensorValue[GYRO_X] += mpu.getRotationX();
		}
		for (int i=0; i<3; i++) sensorValue[i] = sensorValue[i]/5 - sensorZero[i];
	
		// convert IMU values
		accY = sensorValue[ACC_Y]/16384.0;
		accZ = sensorValue[ACC_Z]/16384.0;
		gyroX = sensorValue[GYRO_X]/131.0;


		if (skip++==5) {
			skip = 0;
			//	printf("sensorValue[ACC_Z]=%10d\tsensorValue[ACC_Y]=%10d\tsensorValue[GYRO_X]=%10d\n",sensorValue[ACC_Z],sensorValue[ACC_Y],sensorValue[GYRO_X]);
			//	printf("accY=%10.2f\taccZ=%10.2f\tgyroX=%10.2f\n",accY,accZ,gyroX);
			//	printf("lastLoopUsefulTime=%10d, lastLoopTime=%10d\n",lastLoopUsefulTime,lastLoopTime);
		}


		PID(motorPower);

		// drive motor
		pthread_mutex_lock (&lock);
		robot->writePWM(motorPower,motorPower);
		pthread_mutex_unlock (&lock);


		// code to make sure loop executes with precise timing
		//printf("lastLoopUsefulTime=%10d loopStartTime=%10lu lastLoopTime=%10d\n",lastLoopUsefulTime,loopStartTime,lastLoopTime);
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
	RClaw* robot = (RClaw *)robot_;

	while (!break_condition) {
		scanf("%c",&q);
		if (q == 'q') {
			break_condition = true;
			printf("Breaking out of loop\n");

			pthread_mutex_lock (&lock);
			delete robot;
			pthread_mutex_unlock (&lock);
		}
		fflush(stdin);
	}
	return NULL;
}

int main(int argc, char** argv)
{
	if (argc != 6) {
		fprintf(stderr,"usage: sudo %s Kp Ki Kd PRINT_ANGLE DEBUG\n",argv[0]);
		fprintf(stderr,"\tsudo %s 60 .5 .5 0 0\n",argv[0]);;
		return 1;
	}
	Kp = atof(argv[1]);
	Ki = atof(argv[2]);
	Kd = atof(argv[3]);
	PRINT_ANGLE = atoi(argv[4]);
	DEBUG = atoi(argv[5]);

	// initialize everything
	int address=0x80; //address of roboclaw unit
	int baudrate=38400;
	string tty = "/dev/serial0";
	RClaw *robot = new RClaw (tty, baudrate, address);




	// multithreading
	pthread_t loop_thread, break_thread;

	pthread_mutex_init (&lock, NULL);
	pthread_create (&loop_thread, NULL, Balance, robot);
	pthread_create (&break_thread, NULL, Stop, robot);

	int pterr1 = pthread_join (loop_thread, NULL);
	int pterr2 = pthread_join (break_thread, NULL);
	pterr1 ? fprintf(stderr,"loop_thread error %d\n",pterr1) : printf("loop_thread joined\n");
	pterr2 ? fprintf(stderr,"break_thread error %d\n",pterr2) : printf("break_thread joined\n");

	return 0;
}

