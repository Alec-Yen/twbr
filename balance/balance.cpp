#include <cstdlib>
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <thread>
#include "PiMotor.h"
#include "MPU6050.h"
#include "I2Cdev.h"

using namespace std;

// global variables
bool DEBUG = 1; // set to 1 to avoid running the motors (testing only angle)
int p1 = 18; //Left PWM (refers to BCM numbers "GPIO 18", not the physical pins)
int d1 = 23; //Left DIR
int p2 = 12; //Right PWM
int d2 = 16; //Right DIR
double sampleTime = 0.01; // seconds
double targetAngle = 0; // FIX: this is how the imu is set if you push it all the way down
double Kp; 
double Kd; 
double Ki; 
double RAD_TO_DEG = 180.0/3.14;
int MAX_MOTOR = 100;
double iTerm = 0;
double prevAngle = 0;
bool break_condition = false;

// FIX: try to not to make global
TWBR robot(p1,d1,p2,d2);
double accY, accZ, gyroX;

void PID (double& motorPower, int& direction)
{
	double err,gyroRate,changeInAngle,pTerm,dTerm;
	double accAngle, gyroAngle, currentAngle;
	
	// calculate the angle of inclination
	accAngle = (double) atan2(accY, accZ) * RAD_TO_DEG; // degrees
	gyroRate = gyroX; // degrees/second
	gyroAngle = gyroRate*sampleTime; // degrees - FIX: should make more precise in timing
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

	// determine direction and get magnitude of power
	if (motorPower < 0) {
		direction = 1;
		motorPower *= -1;
	}
	else direction = 0;

	// print statements
	printf("accAngle %.2f\t gyroAngle %.6f\t\t currentAngle %.2f\n",accAngle,gyroAngle,currentAngle);
	printf("pTerm = %.2f\t iTerm = %.2f\t dTerm = %.2f\t motorPower = %.2f\n",pTerm,iTerm,dTerm,motorPower);

	if (DEBUG) {
		motorPower = 0; //DEBUGGING: for testing without running motors
		return;
	}

}


void Balance ()
{
	I2Cdev::initialize();
	MPU6050 mpu;
	mpu.initialize();

	double motorPower;
	int direction;

	while(!break_condition) {
	//	mpu.getMotion6(&ax,&ay,&az,&gz,&gy,&gz);
		accY = mpu.getAccelerationY()/16384.0;
		accZ = mpu.getAccelerationZ()/16384.0;
		gyroX = mpu.getRotationX()/131.0;
		PID(motorPower,direction);
		robot.moveSame(direction,motorPower,sampleTime*1000); // third argument is in milliseconds
	}
}

void Stop ()
{
	char q;
	cin >> q;
	if (q == 'q') {
		break_condition = true;
		cout << "Breaking out of loop\n";
		robot.stop(); // also calls gpioTerminate()
	}
}

int main(int argc, char** argv)
{

	if (argc != 4) {
		cerr << "usage: ./balance Kp Ki Kd\n";
		return 1;
	}
	Kp = atof(argv[1]);
	Ki = atof(argv[2]);
	Kd = atof(argv[3]);

	thread loop_thread (Balance);
	thread break_thread (Stop);

	loop_thread.join();
	break_thread.join();

	cout << "Threads joined successfully\n";

	return 0;
}
