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
//get angle function 

// global variables
bool PRINT_ANGLE = 1; // set to 1 to print out angles and PID terms
bool DEBUG = 0; // set to 1 to avoid running the motors (testing only angle)
double targetAngle = 0;
double RAD_TO_DEG = 180.0/3.14159;
int MAX_MOTOR = 100; // out of 100, TODO: trying to figure out optimal value, might be 100 
float Q_angle  =  0.001; 
float Q_gyro   =  0.003;
float R_angle  =  0.03;  
float x_angle = 0;
float x_bias = 0;
float P_00 = 0, P_01 = 0, P_10 = 0, P_11 = 0;
float dt, y, S;
float K_0, K_1;



// for timing
double sampleTime = 0.01; // in seconds, or 10 milliseconds
int STD_LOOP_TIME = 10; // in milliseconds TODO: Kas does 9 for some reason           
int lastLoopTime = STD_LOOP_TIME;
int lastLoopUsefulTime = STD_LOOP_TIME;
unsigned long loopStartTime;

// for PID method
double Kp, Kd, Ki;  // PID coefficients
double accY, accZ, gyroX; // IMU measurements
double iTerm = 0;
double prevAngle = 0;

// multithreading shared variables
pthread_mutex_t lock; // for thread safe code
bool break_condition = false;

//calculating angle
float kalmanCalculate(float newAngle, float newRate,int looptime) {
	dt = float(looptime)/1000;
	x_angle += dt * (newRate - x_bias);
	P_00 +=  - dt * (P_10 + P_01) + Q_angle * dt;
	P_01 +=  - dt * P_11;
	P_10 +=  - dt * P_11;
	P_11 +=  + Q_gyro * dt;

	y = newAngle - x_angle;
	S = P_00 + R_angle;
	K_0 = P_00 / S;
	K_1 = P_10 / S;

	x_angle +=  K_0 * y;
	x_bias  +=  K_1 * y;
	P_00 -= K_0 * P_00;
	P_01 -= K_0 * P_01;
	P_10 -= K_1 * P_00;
	P_11 -= K_1 * P_01;

	return x_angle;
}

// calculate motor PWM from PID equation
void PID (double& motorPower)
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
	iTerm += Ki*err;
	dTerm = Kd*changeInAngle;

	motorPower = pTerm + iTerm + dTerm;
	prevAngle = currentAngle;

	// keep within max power
	if (motorPower > MAX_MOTOR) motorPower = MAX_MOTOR;
	else if (motorPower < -MAX_MOTOR ) motorPower = -MAX_MOTOR;

	// TODO: had to include to fix it
	motorPower *= -1;


	// print statements
	if (PRINT_ANGLE) printf("accAngle %.2f\t gyroAngle %.6f\t\t currentAngle %.2f\n",accAngle,gyroAngle,currentAngle);
	//	if (PRINT_ANGLE) printf("pTerm = %.2f\t iTerm = %.2f\t dTerm = %.2f\t motorPower = %.2f\n",pTerm,iTerm,dTerm,motorPower);

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

	loopStartTime = millis();
	while(!break_condition) {

		// balance code
		accY = mpu.getAccelerationY()/16384.0;
		accZ = mpu.getAccelerationZ()/16384.0;
		gyroX = mpu.getRotationX()/131.0;
		PID(motorPower);

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


