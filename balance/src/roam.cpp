// Balancing code that uses precise timing, averages imu readings, uses encoder values, removes calibration
// This code doesn't zero out angle readings before running

#include <stdlib.h> // basic libraries
#include <stdio.h>
#include <unistd.h>

#include <math.h> // for atan
#include <pthread.h> // for multithreading
#include "twbr.h" // for TWBR methods TODO: should be switched to wiringPi, not PIGPIO, to reduce dependencies
#include "MPU6050.h" // for MPU6050 methods
#include "I2Cdev.h"  // for MPU6050 methods
#include "timing.h" // for delay()
#include "encoder.h" // for encoder methods

#include <signal.h> // TODO: not sure we need this
#include <time.h> // TODO: not sure we need this

using namespace std;

// global variables
bool PRINT = 1; // set to 1 to print out angles and PID terms
bool DEBUG = 0; // set to 1 to avoid running the motors (testing only angle)
int p1 = 18;  //Left PWM (refers to BCM numbers "GPIO 18", not the physical pins)
int d1 = 23;  //Left DIR
int p2 = 12;  //Right PWM
int d2 = 16;  //Right DIR
int e1a = 17; //Encoder 1A
int e1b = 27; //Encoder 1B
int e2a = 5;  //Encoder 2A
int e2b = 6;  //Encoder 2B

double targetAngle = 0;
double RAD_TO_DEG = 180.0/3.14159;
int MAX_MOTOR = 200; // max is 255

// for PID method
double Kp, Kd, Ki, Kp_dx, Kd_dx; // PID coefficients 
double accX, accZ, gyroY; // IMU measurements
long enc1_val, enc2_val; // encoder values
double iTerm = 0;
double prevAngle = 0;
double prevEnc = 0;

// for precise timing
int STD_LOOP_TIME = 10; // in milliseconds
int lastLoopTime = STD_LOOP_TIME;
int lastLoopUsefulTime = STD_LOOP_TIME;
unsigned long loopStartTime;
double sampleTime = STD_LOOP_TIME/1000.0; // in seconds

// for IMU calibration
enum IMU_Index {ACC_X,ACC_Z,GYRO_Y};
int sensorZero[3] = {0,-16384,0}; // calibration
int sensorValue[3] = {0,0,0}; // averaged raw IMU measurements

// for encoders
struct encoder *enc1 = setupencoder(e1a,e1b);
struct encoder *enc2 = setupencoder(e2a,e2b);

// multithreading shared variables
pthread_mutex_t lock; // for thread safe code
bool break_condition = false;


// calculate motor PWM from PID equation
void PID (double& motorPower, int& direction)
{
	double err_angle,gyroRate;
	double accAngle, gyroAngle, currentAngle;
	long currentEnc;

	double pTerm,dTerm,pTerm_dx,dTerm_dx;

	// calculate the angle of inclination
	accAngle = (double) atan2(accX, accZ) * RAD_TO_DEG; // degrees
	gyroRate = gyroY; // degrees/second
	gyroAngle = gyroRate*sampleTime; // degrees
	currentAngle = 0.99*(prevAngle + gyroAngle) + 0.01*(accAngle); // complementary filter

	// angle PID calculations
	pthread_mutex_lock (&lock);
	err_angle = currentAngle - targetAngle; // targetAngle is 0
	pthread_mutex_unlock (&lock);

	pTerm = Kp*err_angle;
	iTerm += Ki*err_angle;
	dTerm = Kd*(currentAngle - prevAngle);
	prevAngle = currentAngle;

	// encoder PD calculations
	currentEnc = (enc1_val+enc2_val)/2;
	pTerm_dx = Kp_dx*currentEnc;
	dTerm_dx = Kd_dx*(currentEnc-prevEnc);
	prevEnc = currentEnc;

	// sum up to motorPower
	motorPower = pTerm + iTerm + dTerm + pTerm_dx + dTerm_dx;
	motorPower *= -1; // TODO: make this more readable


	// keep within max power
	if (motorPower > MAX_MOTOR) motorPower = MAX_MOTOR;
	else if (motorPower < -MAX_MOTOR ) motorPower = -MAX_MOTOR;


	// print statements
	//if (PRINT) printf("currentAngle %3.2f\t enc1 = %ld\t enc2 = %ld\t enc_av = %ld\t motorPower = %3.2f\n",currentAngle,enc1_val,enc2_val,currentEnc,motorPower);
	if (PRINT) printf("currAng = %6.2f currEnc = %6ld\t pTerm = %6.2f iTerm = %6.2f dTerm = %6.2f pTerm_dx = %6.2f dTerm_dx = %6.2f motorPower = %6.2f\n",currentAngle,currentEnc,pTerm, iTerm, dTerm, pTerm_dx,dTerm_dx,motorPower);


	// determine direction and get magnitude of power
	if (motorPower < 0) {
		direction = 0;
		motorPower *= -1;
	}
	else direction = 1;

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

	// initialize everything
	I2Cdev::initialize();
	MPU6050 mpu;
	mpu.initialize();

	delay(100); // wait a moment
	int AVERAGE_TIMES = 5; // number of values averaged together for imu

	// balancing loop
	loopStartTime = millis();
	while(!break_condition) {

		// read and average IMU values
		for (int i=0; i<3; i++) sensorValue[i] = 0;
		for (int n=0; n<AVERAGE_TIMES; n++) {
			sensorValue[ACC_X] += mpu.getAccelerationX();
			sensorValue[ACC_Z] += mpu.getAccelerationZ();
			sensorValue[GYRO_Y] += mpu.getRotationY();
		}
		for (int i=0; i<3; i++) sensorValue[i] = sensorValue[i]/AVERAGE_TIMES - sensorZero[i];

		accX = sensorValue[ACC_X]/16384.0; // TODO: probably don't need to divide by this
		accZ = sensorValue[ACC_Z]/16384.0;
		gyroY = -1*sensorValue[GYRO_Y]/131.0;

		// read encoder values
		updateEncoders();
		enc1_val = enc1->value;
		enc2_val = -1*enc2->value;

		// call PID functions and write to motors
		PID(motorPower,direction);
		pthread_mutex_lock (&lock);
		robot->writePWMSame(direction,motorPower);
		pthread_mutex_unlock (&lock);

		// code to make sure loop executes with precise timing of every STD_LOOP_TIME milliseconds
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

	while(!break_condition) {
		scanf("%c",&q);
		if (q == 'q') {
			break_condition = true;
			printf("Breaking out of loop\n");

			pthread_mutex_lock (&lock);
			delay(100);
			robot->writePWMSame(1,0);
			pthread_mutex_unlock (&lock);
		}
		fflush(stdin);
	}
	return NULL;
}

void* Roam (void* robot_)
{
	char q;
	TWBR* robot = (TWBR *)robot_;

	while (!break_condition) {
		scanf ("%c",&q);
		if (q == 'f') { // move forward
			pthread_mutex_lock (&lock);
			targetAngle = .6;
			printf("Moving with target angle %.2f degrees\n",targetAngle);
			pthread_mutex_unlock (&lock);
		}
		else if (q == 'b') { // move backward
			pthread_mutex_lock (&lock);
			targetAngle = -.6;
			printf("Moving with target angle %.2f degrees\n",targetAngle);
			pthread_mutex_unlock (&lock);
		}
		else if (q == 's') { // stand still
			pthread_mutex_lock (&lock);
			targetAngle = 0;
			printf("Moving with target angle %.2f degrees\n",targetAngle);
			pthread_mutex_unlock (&lock);
		}

		else if (q == 'q') { // stop robot
			break_condition = true;
			printf("Breaking out of loop\n");
			pthread_mutex_lock (&lock);
			delay(100);
			robot->writePWMSame(1,0);
			pthread_mutex_unlock (&lock);
		}

		fflush(stdin);
	}
	return NULL;
}

// main function
int main(int argc, char** argv)
{
	// check command line arguments
	if (argc != 8) {
		fprintf(stderr,"usage: sudo %s Kp Ki Kd Kp_dx Kd_dx PRINT DEBUG\n",argv[0]);
		fprintf(stderr,"\tsudo %s 90 10 90 -.6 -.5\n",argv[0]);
		return 1;
	}
	Kp = atof(argv[1]);
	Ki = atof(argv[2]);
	Kd = atof(argv[3]);
	Kp_dx = atof(argv[4]);
	Kd_dx = atof(argv[5]);
	PRINT = atoi(argv[6]);
	DEBUG = atoi(argv[7]);

	// initialize everything
	TWBR *robot = new TWBR(p1,d1,p2,d2);

	// multithreading
	pthread_t loop_thread, break_thread;

	pthread_mutex_init (&lock, NULL);
	pthread_create (&loop_thread, NULL, Balance, robot);	
	pthread_create (&break_thread, NULL, Roam, robot);
	pthread_join (loop_thread, NULL);
	pthread_join (break_thread, NULL);
	printf("Threads joined successfully\n");
	robot->writePWMSame(1,0);

	return 0;
}
