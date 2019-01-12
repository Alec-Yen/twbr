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
int MAX_MOTOR = 100;

// for PID method
double Kp, Kd, Ki; // PID coefficients 
double accY, accZ, gyroX; // IMU measurements
double iTerm = 0;
double prevAngle = 0;
clock_t prev_t;
double motorTime = 0.01; // seconds

// multithreading shared variables
pthread_mutex_t lock; // for thread safe code
bool break_condition = false;


// calculate motor PWM from PID equation
void PID (double& motorPower, int& direction)
{
	double err,gyroRate,changeInAngle,pTerm,dTerm;
	double accAngle, gyroAngle, currentAngle;
	double sampleTime;
	
	// calculate sampleTime (TODO: calculations not working)
	sampleTime = 0.01;
	//clock_t curr_t = clock();
	//sampleTime = (double)(curr_t-prev_t)/CLOCKS_PER_SEC;
	//prev_t = curr_t;


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
		direction = 1;
		motorPower *= -1;
	}
	else direction = 0;

	// print statements
	if (PRINT) printf("accAngle %.2f\t gyroAngle %.6f\t\t currentAngle %.2f\n",accAngle,gyroAngle,currentAngle);
	if (PRINT) printf("pTerm = %.2f\t iTerm = %.2f\t dTerm = %.2f\t motorPower = %.2f\n",pTerm,iTerm,dTerm,motorPower);

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

	while(!break_condition) {
		accY = mpu.getAccelerationY()/16384.0;
		accZ = mpu.getAccelerationZ()/16384.0;
		gyroX = mpu.getRotationX()/131.0;
		PID(motorPower,direction);

		pthread_mutex_lock (&lock);
		robot->moveSame(direction,motorPower,motorTime*1000); // third argument is in milliseconds
		pthread_mutex_unlock (&lock);
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
		robot->wait(1000);
		robot->stop(); // also calls gpioTerminate()
		pthread_mutex_unlock (&lock);
	}
	return NULL;
}

// main function
int main(int argc, char** argv)
{
	// check command line arguments
	if (argc != 4) {
		fprintf(stderr,"usage: sudo %s Kp Ki Kd\n",argv[0]);
		fprintf(stderr,"\tsudo %s 60 .5 .5\n",argv[0]);
		return 1;
	}
	Kp = atof(argv[1]);
	Ki = atof(argv[2]);
	Kd = atof(argv[3]);

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

	return 0;
}
