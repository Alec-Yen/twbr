// Program to balance robot that worked at end of Fall 2018 using MDD10A
// Dependencies: pigpio, bcm2835, c++11

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <thread>
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
bool break_condition = false;
TWBR robot(p1,d1,p2,d2);


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
	err = currentAngle - targetAngle; // targetAngle is 0
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
void Balance ()
{
	double motorPower;
	int direction;

	I2Cdev::initialize();
	MPU6050 mpu;
	mpu.initialize();

	while(!break_condition) {
		accY = mpu.getAccelerationY()/16384.0;
		accZ = mpu.getAccelerationZ()/16384.0;
		gyroX = mpu.getRotationX()/131.0;
		PID(motorPower,direction);
		robot.moveSame(direction,motorPower,motorTime*1000); // third argument is in milliseconds
	}
}

// stop robot upon entering 'q'
void Stop ()
{
	char q;
	cin >> q;
	if (q == 'q') {
		break_condition = true;
		cout << "Breaking out of loop\n";
		robot.wait(1000);
		robot.stop(); // also calls gpioTerminate()
	}
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

	// initialize prev_t
	prev_t = clock();

	// multithreading
	thread loop_thread (Balance);
	thread break_thread (Stop);

	loop_thread.join();
	break_thread.join();
	cout << "Threads joined successfully\n";

	return 0;
}
