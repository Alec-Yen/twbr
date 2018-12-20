#include <cstdlib>
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include "PiMotor.h"
#include "MPU6050.h"
#include "I2Cdev.h"

using namespace std;

// supposed to be global (FIX: maybe change to define later)
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

// FIX: don't make these global 
double accY, accZ, gyroX;
double accAngle, gyroAngle, currentAngle, prevAngle=0;
double err, prev_error=0, error_sum=0;
int count = 0;
int distanceCm;
double iTerm = 0;
TWBR robot(p1,d1,p2,d2);

// sigint handling - NOT WORKING
static volatile bool keepRunning = true;
void intHandler(int signum) {
	keepRunning = false;
	robot.wait(1000);
	cout << "intHandler called\n";
}

void PID (double& motorPower, int& direction)
{
	double gyroRate, changeInAngle,pTerm,dTerm;

	// calculate the angle of inclination
	accAngle = (double) atan2(accY, accZ) * RAD_TO_DEG; // degrees
	gyroRate = gyroX; // degrees/second
	gyroAngle = gyroRate*sampleTime; // degrees - FIX: should make more precise in timing

	currentAngle = 0.99*(prevAngle + gyroAngle) + 0.01*(accAngle); // complementary filter
	
	err = currentAngle - targetAngle; // targetAngle is 0
	changeInAngle = currentAngle - prevAngle;
	pTerm = Kp*err;
	iTerm += Ki*err*sampleTime;
	dTerm = Kd*changeInAngle/sampleTime;

//	printf("gyroRate = %.2f\t gyroAngle %.6f\n",gyroRate, gyroAngle);
	printf("accAngle %.2f\t gyroAngle %.6f\t\t currentAngle %.2f\n",accAngle,gyroAngle,currentAngle);

	motorPower = pTerm + iTerm + dTerm;
	printf("pTerm = %.2f\t iTerm = %.2f\t dTerm = %.2f\t",pTerm,iTerm,dTerm);


	prevAngle = currentAngle;

//	motorPower = 0; //DEBUGGING: for testing without running motors
//	return;

	if (motorPower > MAX_MOTOR) motorPower = MAX_MOTOR;
	else if (motorPower < -MAX_MOTOR ) motorPower = -MAX_MOTOR;

	printf("motorPower %.2f\n",motorPower);

	if (motorPower < 0) {
		direction = 1;
		motorPower *= -1;
	}
	else direction = 0;

}

int main(int argc, char** argv) {

	I2Cdev::initialize();
	MPU6050 mpu;
	mpu.initialize();

	if (argc != 4) {
		cerr << "usage: ./balance Kp Ki Kd\n";
		return 1;
	}
	Kp = atof(argv[1]);
	Ki = atof(argv[2]);
	Kd = atof(argv[3]);

	double motorPower;
	int direction;

	signal (SIGINT,intHandler);

	while (keepRunning) {
	//	mpu.getMotion6(&ax,&ay,&az,&gz,&gy,&gz);
		accY = mpu.getAccelerationY()/16384.0;
		accZ = mpu.getAccelerationZ()/16384.0;
		gyroX = mpu.getRotationX()/131.0;
		PID(motorPower,direction);
		robot.moveSame(direction,motorPower,sampleTime*1000); // third argument is in milliseconds
	}


	return 0;
}



//constrain
//	if (error_sum > 300) error_sum = 300;
//	else if (error_sum < -300) error_sum = -300;

	//calculate output from P, I and D values
//	Pterm = Kp*(currentAngle-targetAngle);
//	iTerm += Ki*currentAngle;
//	dTerm = Kd * (currentAngle-prevAngle);
//	motorPower = Pterm + iTerm + dTerm;

//	motorPower = Kp*(err) + Ki*(error_sum)*sampleTime - Kd*(currentAngle-prevAngle)/sampleTime; // comment out?

//	motorPower = (Kp*err + (Kd*1000*err/sampleTime) + Ki*error_sum);
//	printf("motorPower = %.2f*%.2f + (%.2f*1000*%.2f/%.2f) + %.2f*%.2f = %.2f\n",Kp,err,Kd,err,sampleTime,Ki,error_sum,motorPower);
	
