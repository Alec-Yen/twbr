#include <cstdlib>
#include <stdio.h>
#include "PiMotor.h"

using namespace std;

int p1 = 18; //Motor 0 PWM (refers to BCM numbers "GPIO 18", not the physical pins)
int d1 = 23; //Motor 0 DIR
int p2 = 12;
int d2 = 16;

int main() {
    //Create a new instance for our Motor.

		TWBR robot(p1,d1,p2,d2);
		robot.moveSame (0,0,1000);
//		robot.wait(100);
//		robot.moveSame (1,20,1000);
    
   
    return 0;
}

