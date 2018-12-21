#include "roboclaw.h"
#include <stdio.h>  //printf
#include <stdlib.h> //exit
#include <unistd.h> //sleep


int main(int argc, char **argv)
{
	struct roboclaw *rc;
	uint8_t address=0x80; //address of roboclaw unit
	int baudrate=38400;

	int duty_cycle;
	int enc1, enc2;

	rc=roboclaw_init("/dev/serial0", baudrate);

	if( rc == NULL )
	{
		perror("Unable to initialize roboclaw");
		exit(1);
	}

	printf("Initialized communication with roboclaw\n");

	while(1) {
		printf ("PWM Input: ");
		if (scanf("%d",&duty_cycle) == 1) {
			if (duty_cycle == 0) break;
			if (duty_cycle > 100) duty_cycle = 100;
			if (duty_cycle < -100) duty_cycle = -100;

			//32767 is max duty cycle setpoint that roboclaw accepts
			duty_cycle = (float)duty_cycle/100.0f * 32767;	

			if(roboclaw_duty_m1m2(rc, address, duty_cycle, duty_cycle) != ROBOCLAW_OK ) {
				fprintf(stderr, "Problem communicating with roboclaw, terminating\n");
				break;
			}
			roboclaw_encoders(rc, address, &enc1, &enc2);
			printf("Encoder: %d\n", enc1);
		}
	}

	//make sure the motors are stopped before leaving
	printf("Stopping the motors..");
	roboclaw_duty_m1m2(rc, address, 0, 0);

	if(roboclaw_close(rc) != ROBOCLAW_OK) perror("Unable to shutdown roboclaw cleanly\n");
	else printf("Clean exit\n");

	return 0;
}
