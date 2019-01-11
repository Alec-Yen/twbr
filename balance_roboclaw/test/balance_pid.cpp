#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include "rc_twbr.hpp"
#include "MPU6050.h"
#include "I2Cdev.h"
#include "pid.h"

using namespace std;

// global variables
bool PRINT_ANGLE = 1; // set to 1 to print out angles and PID terms
bool DEBUG = 0; // set to 1 to avoid running the motors (testing only angle)
double targetAngle = 0;
double RAD_TO_DEG = 180.0/3.14159;
int MAX_MOTOR = 60; // TODO: this value is going to be lower using roboclaw since it is out of 100
int count = 0; // TODO: don't want to have to include

// for PID method
double Kp, Kd, Ki;  // PID coefficients
double accY, accZ, gyroX; // IMU measurements
double prevAngle = 0;
clock_t prev_t;
double motorTime = 0.01; // seconds

// multithreading shared variables
pthread_mutex_t lock; // for thread safe code
bool break_condition = false;





// calculate motor PWM from PID equation
void PID (SPid *spid, double& motorPower)
{
	double err,gyroRate;
	double accAngle, gyroAngle, currentAngle;
	double sampleTime;
	
	// calculate sampleTime (TODO: calculations not working)
	sampleTime = 0.01;
//	clock_t curr_t = clock();
//	sampleTime = (double)(curr_t-prev_t)/CLOCKS_PER_SEC;
//	prev_t = curr_t;
//	printf("sampleTime %.6f\n",sampleTime);


	// calculate the angle of inclination
	accAngle = (double) atan2(accY, accZ) * RAD_TO_DEG; // degrees
	gyroRate = gyroX; // degrees/second
	gyroAngle = gyroRate*sampleTime; // degrees
	currentAngle = 0.99*(prevAngle + gyroAngle) + 0.01*(accAngle); // complementary filter
	
	// PID calculations
	pthread_mutex_lock (&lock);
	err = currentAngle - targetAngle; // targetAngle is 0
	pthread_mutex_unlock (&lock);
//	changeInAngle = currentAngle - prevAngle;
//	pTerm = Kp*err;
//	iTerm += Ki*err*sampleTime;
//	dTerm = Kd*changeInAngle/sampleTime;

	motorPower = spid->UpdatePID(err, currentAngle);
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

	SPid* spid = new SPid;
	spid->derState = 0;
	spid->integratState = 0;
	spid->integratMax = 10;
	spid->integratMin = -spid->integratMax;
	spid->integratGain = Ki;
	spid->propGain = Kp;
	spid->derGain = Kd;

	while(!break_condition) {
		accY = mpu.getAccelerationY()/16384.0;
		accZ = mpu.getAccelerationZ()/16384.0;
		gyroX = mpu.getRotationX()/131.0;
		PID(spid, motorPower);

		pthread_mutex_lock (&lock);
		robot->moveSame(motorPower,motorTime*1000); // third argument is in milliseconds
		pthread_mutex_unlock (&lock);
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
	prev_t = clock();
	
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
