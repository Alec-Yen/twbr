#include "rc_twbr.hpp"
#include <stdio.h>  //printf
#include <stdlib.h> //exit
#include <unistd.h> //sleep

using namespace std;

int main(int argc, char **argv)
{
	uint8_t address=0x80; //address of roboclaw unit
	int baudrate=38400;
	string tty = "/dev/serial0";

	int duty_cycle;
	int enc1, enc2;

	TWBR *robot = new TWBR (tty, baudrate, address);

	while(1) {
		printf ("PWM Input: ");
		if (scanf("%d",&duty_cycle) == 1) {
			if (duty_cycle == 0) break;
			if (duty_cycle > 100) duty_cycle = 100;
			if (duty_cycle < -100) duty_cycle = -100;

			robot->moveSame (duty_cycle,1000);
			robot->readEncoders (enc1,enc2);
			printf("Encoder: %d\n", enc1);
		}
	}
	delete robot;

	return 0;
}
