#include <cstdlib>
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include "MPU6050.h"
#include "I2Cdev.h"

using namespace std;

double RAD_TO_DEG = 180.0/3.14159;
double prevAngle = 0;
double sampleTime = 0.01;


int main()
{
	I2Cdev::initialize();
	MPU6050 mpu;
	mpu.initialize();

	int count = 0;
	while (1) {
		double accX = mpu.getAccelerationX()/16384.0;
		double accY = mpu.getAccelerationY()/16384.0;
		double accZ = mpu.getAccelerationZ()/16384.0;
		double gyroX = mpu.getRotationX()/131.0;
		double gyroY = mpu.getRotationY()/131.0;
		double gyroZ = mpu.getRotationZ()/131.0;

		// calculate angle
		double accAngle = (double) atan(accX/accZ) * RAD_TO_DEG; //degrees
		double gyroRate = -gyroY; // degrees/second
		double gyroAngle = gyroRate*sampleTime; // degrees
		double currentAngle = 0.99*(prevAngle + gyroAngle) + 0.01*accAngle; // complementary filter
		prevAngle = currentAngle;

		count++;
		if (count%100==0) printf ("accX=%8.2f accY=%8.2f accZ=%8.2f gyroX=%8.2f gyroY=%8.2f gyroZ=%8.2f accAngle=%8.2f gyroAngle=%8.2f currentAngle=%8.2f\n", accX, accY, accZ, gyroX, gyroY, gyroZ, accAngle, gyroAngle, currentAngle); 

		sleep(sampleTime);
	}

	return 0;
}
