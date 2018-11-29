#include <cstdlib>
#include <stdio.h>
#include <cmath>
#include "PiMotor.h"
#include "MPU6050.h"

using namespace std;

int p1 = 18; //Left PWM (refers to BCM numbers "GPIO 18", not the physical pins)
int d1 = 23; //Left DIR
int p2 = 12; //Right PWM
int d2 = 16; //Right DIR

enum {vl,vm,vr} typedef v_dir;
enum {angl,angm,angr} typedef ang_dir;

// returns currentAngle
double PID (double accY, double accZ, double gyroX, double& prevAngle)
{

}

int main(int argc, char** argv) {

	double accY, accZ, gyroX;
	double accAngle, gyroAngle, currentAngle, prevAngle=0;
	double err, prevError=0, errorSum=0;
	int count = 0;
	int distanceCm;


	TWBR robot(p1,d1,p2,d2);
	MPU6050 mpu;

	while (1) {
		accY = mpu.GetAccelerationY();
		accZ = mpu.GetAcclerationZ();
		gyroX = mpu.GetRotationX();
			
	}
	robot.moveSame (0,20,1000);
	robot.wait(100);
	robot.moveSame (1,20,1000);


	return 0;
}

