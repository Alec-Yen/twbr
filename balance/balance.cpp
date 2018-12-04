#include <cstdlib>
#include <stdio.h>
#include <cmath>
#include "PiMotor.h"
#include "MPU6050.h"
#include "I2Cdev.h"

using namespace std;

// not using right now
enum {vl,vm,vr} typedef v_dir;
enum {angl,angm,angr} typedef ang_dir;

// supposed to be global (FIX: maybe change to define later)
int p1 = 18; //Left PWM (refers to BCM numbers "GPIO 18", not the physical pins)
int d1 = 23; //Left DIR
int p2 = 12; //Right PWM
int d2 = 16; //Right DIR
double sampleTime = 0.005; // seconds
double targetAngle = 0;
double Kp = 40;
double Kd = 0.05;
double Ki = 40;
double RAD_TO_DEG = 180.0/3.14;

// FIX: don't make these global (copying Arduino code right now)
double accY, accZ, gyroX;
double accAngle, gyroAngle, currentAngle, prevAngle=0;
double err, prev_error=0, error_sum=0;
int count = 0;
int distanceCm;



void PID (double& motorPower, int& direction)
{

	double gyroRate;

	// calculate the angle of inclination
	accAngle = atan2(accY, accZ) * RAD_TO_DEG;
	//gyroRate = map(gyroX, -32768, 32767, -250, 250);
	gyroRate = gyroX * 250/32768 * RAD_TO_DEG;


	gyroAngle = gyroRate*sampleTime;  
	currentAngle = 0.9934*(prevAngle + gyroAngle) + 0.0066*(accAngle); // complementary filter
	//currentAngle = 0.0066*(prevAngle + gyroAngle) + 0.9934*(accAngle); // complementary filter

	err = currentAngle - targetAngle;
	error_sum = error_sum + err;

	printf("accAngle %.2f\t gyroAngle %.6f\t\t gyroRate %.2f\t currentAngle %.2f\n",accAngle,gyroAngle,gyroRate,currentAngle);

	//constrain
	if (error_sum > 300) error_sum = 300;
	else if (error_sum < -300) error_sum = -300;

	//calculate output from P, I and D values
	motorPower = Kp*(err) + Ki*(error_sum)*sampleTime - Kd*(currentAngle-prevAngle)/sampleTime;
	prevAngle = currentAngle;

/*	// toggle the led on pin13 every second
	count++;
	if(count == 200)  {
		count = 0;
		digitalWrite(13, !digitalRead(13));
	}
*/
	// constrain (we determined max reasonable to be 200)
	motorPower = 0; //DELETE LATER
	return;

	if (motorPower > 100) motorPower = 100;
	else if (motorPower < -100 ) motorPower = -100;

	if (motorPower < 0) {
		direction = 0;
		motorPower *= -1;
	}
	else direction = 1;

}

int main(int argc, char** argv) {

	TWBR robot(p1,d1,p2,d2);
	I2Cdev::initialize();
	MPU6050 mpu;
	mpu.initialize();

	double motorPower;
	int direction;
	int16_t ax, ay, az, gx, gy, gz;

	while (1) {
	//	mpu.getMotion6(&ax,&ay,&az,&gz,&gy,&gz);
		accY = mpu.getAccelerationY()/16384.0;
		accZ = mpu.getAccelerationZ()/16384.0;
		gyroX = mpu.getRotationX()/131.0;
//		printf("accY = %.2f accZ = %.2f gyroX = %.2f\n",accY,accZ,gyroX);
		PID(motorPower,direction);
		robot.moveSame(direction,motorPower,100);
		robot.wait(sampleTime*1000);
	}


	return 0;
}

