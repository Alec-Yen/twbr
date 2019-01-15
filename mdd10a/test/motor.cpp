#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include "PiMotor.h"

using namespace std;

int p1 = 18; //Motor 0 PWM (refers to BCM numbers "GPIO 18", not the physical pins)
int d1 = 23; //Motor 0 DIR
int p2 = 12;
int d2 = 16;

int main(int argc, char** argv) {
	//Create a new instance for our Motor.

	int s;
	TWBR robot(p1,d1,p2,d2);
	while (1) {
		if (cin >> s) {
//			robot.moveSame (0,s,1000);
//			wait(100);
//			robot.moveSame (1,s,1000);
			robot.writePWMSame(0,s);
			cin.clear();
		}
	}


	return 0;
}

