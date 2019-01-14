#include <iostream>
#include <cstdio> // for printf
#include "MPU6050.h"
#include "I2Cdev.h"
#include "timing.h"
using namespace std;

// for timing
int STD_LOOP_TIME = 10; // in milliseconds            
int lastLoopTime = STD_LOOP_TIME;
int lastLoopUsefulTime = STD_LOOP_TIME;
unsigned long loopStartTime = millis();

// for IMU calibration
enum IMU_Index {ACC_Y,ACC_Z,GYRO_X};
int sensorZero[3] = {0,0,0}; // calibration
int sensorValue[3] = {0,0,0}; // averaged raw IMU measurements
double accY,accZ,gyroX;

static int skip = 0;

int main()
{
	I2Cdev::initialize();
	MPU6050 mpu;
	mpu.initialize();
	delay(1000);

	// calibrate sensor TODO: see if this works
	for (int n=0; n<50; n++) {
		//mpu.getMotion6 (&sensorTemp[ACC_X],&sensorTemp[ACC_Y],&sensorTemp[ACC_Z],&sensorTemp[GYRO_X],&sensorTemp[GYRO_Y],&sensorTemp[GYRO_Z]);
		sensorZero[ACC_Y] += mpu.getAccelerationY();
		sensorZero[ACC_Z] += mpu.getAccelerationZ();
		sensorZero[GYRO_X] += mpu.getRotationX();
	}
	for (int i=0; i<3; i++) sensorZero[i] /= 50;
	sensorZero[ACC_Z] -= 16384; //TODO: figure out what value to put here (Kas uses 102)

	printf("sensorZero[ACC_Z]=%d\nsensorZero[ACC_Y]=%d\nsensorZero[GYRO_X]=%d\n",sensorZero[ACC_Z],sensorZero[ACC_Y],sensorZero[GYRO_X]);


	// loop
	loopStartTime = millis();
	while (1) {


		// read IMU values TODO: see if this works
		for (int i=0; i<3; i++) sensorValue[i] = 0;
		for (int n=0; n<5; n++) {
			sensorValue[ACC_Y] += mpu.getAccelerationY();
			sensorValue[ACC_Z] += mpu.getAccelerationZ();
			sensorValue[GYRO_X] += mpu.getRotationX();
		}
		for (int i=0; i<3; i++) sensorValue[i] = sensorValue[i]/5 - sensorZero[i];
	
		// convert IMU values
		accY = sensorValue[ACC_Y]/16384.0;
		accZ = sensorValue[ACC_Z]/16384.0;
		gyroX = sensorValue[GYRO_X]/131.0;


		if (skip++==5) {
			skip = 0;
//			printf("sensorValue[ACC_Z]=%10d\tsensorValue[ACC_Y]=%10d\tsensorValue[GYRO_X]=%10d\n",sensorValue[ACC_Z],sensorValue[ACC_Y],sensorValue[GYRO_X]);
			printf("accY=%10.2f\taccZ=%10.2f\tgyroX=%10.2f\n",accY,accZ,gyroX);
			//	printf("lastLoopUsefulTime=%10d, lastLoopTime=%10d\n",lastLoopUsefulTime,lastLoopTime);
		}




		// code to make sure that code is executed precisely every STD_LOOP_TIME 
		lastLoopUsefulTime = millis()-loopStartTime;
		if(lastLoopUsefulTime<STD_LOOP_TIME)			delay(STD_LOOP_TIME-lastLoopUsefulTime);
		lastLoopTime = millis() - loopStartTime;
		loopStartTime = millis();
	}
}
