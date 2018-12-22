#include <cstdlib>
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include "rc_twbr.hpp"
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
double motorTime = 0.01; // seconds
double targetAngle = 0;
double Kp; 
double Kd; 
double Ki; 
double RAD_TO_DEG = 180.0/3.14159;
int MAX_MOTOR = 50; // TODO: this value is going to be lower using roboclaw since it is out of 100
double iTerm = 0;
double prevAngle = 0;
bool break_condition = false;
clock_t prev_t;

// TODO: try to not to make global
double accY, accZ, gyroX;

void PID (double& motorPower)
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

	// max power
	if (motorPower > MAX_MOTOR) motorPower = MAX_MOTOR;
	else if (motorPower < -MAX_MOTOR ) motorPower = -MAX_MOTOR;


	// print statements
	if (PRINT) printf("accAngle %.2f\t gyroAngle %.6f\t\t currentAngle %.2f\n",accAngle,gyroAngle,currentAngle);
	if (PRINT) printf("pTerm = %.2f\t iTerm = %.2f\t dTerm = %.2f\t motorPower = %.2f\n",pTerm,iTerm,dTerm,motorPower);

	if (DEBUG) {
		motorPower = 0; //DEBUGGING: for testing without running motors
		return;
	}

}


void* Balance (void* robot_)
{
	I2Cdev::initialize();
	MPU6050 mpu;
	mpu.initialize();

	double motorPower;
	
	TWBR* robot = (TWBR *)robot_;

	while(!break_condition) {
	//	mpu.getMotion6(&ax,&ay,&az,&gz,&gy,&gz);
		accY = mpu.getAccelerationY()/16384.0;
		accZ = mpu.getAccelerationZ()/16384.0;
		gyroX = mpu.getRotationX()/131.0;
		PID(motorPower);
		robot->moveSame(motorPower,motorTime*1000); // third argument is in milliseconds
	}
	return NULL;
}

void* Stop (void* robot_)
{
	char q;

	TWBR* robot = (TWBR *)robot_;

	cin >> q;
	if (q == 'q') {
		break_condition = true;
		cout << "Breaking out of loop\n";
		sleep(1);
		delete robot;
	}
	return NULL;
}

int main(int argc, char** argv)
{

	pthread_t loop_thread, break_thread;

	if (argc != 4) {
		cerr << "usage: sudo ./bin/balance Kp Ki Kd\n";
		return 1;
	}
	Kp = atof(argv[1]);
	Ki = atof(argv[2]);
	Kd = atof(argv[3]);

	// initialize everything
	uint8_t address=0x80; //address of roboclaw unit
	int baudrate=38400;
	string tty = "/dev/serial0";
	TWBR *robot = new TWBR (tty, baudrate, address);
	prev_t = clock();
	
	// multithreading
	pthread_create (&loop_thread, NULL, Balance, robot);
	pthread_create (&break_thread, NULL, Stop, robot);

	pthread_exit (NULL);

	cout << "Threads joined successfully\n";

	return 0;
}
