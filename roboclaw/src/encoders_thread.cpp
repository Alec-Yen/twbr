#include "roboclaw.h"
#include <stdio.h>  //printf
#include <stdlib.h> //exit
#include <unistd.h> //sleep
#include <pthread.h>

uint8_t address=0x80; //address of roboclaw unit
int baudrate=38400;
bool keepRunning = true;

void* changePWM (void *rc_)
{
	roboclaw *rc = (roboclaw *) rc_;
	int duty_cycle;
	while(keepRunning) {
		if (scanf("%d",&duty_cycle) == 1) {
			if (duty_cycle == 0) {
				keepRunning = false;
				sleep(1);
			}

			if (duty_cycle > 100) duty_cycle = 100;
			if (duty_cycle < -100) duty_cycle = -100;

			//32767 is max duty cycle setpoint that roboclaw accepts
			duty_cycle = (float)duty_cycle/100.0f * 32767;	

			if(roboclaw_duty_m1m2(rc, address, duty_cycle, duty_cycle) != ROBOCLAW_OK ) {
				fprintf(stderr, "Problem communicating with roboclaw, terminating\n");
				keepRunning = false;
				sleep(1);
			}
		}
	}
	
	// close everything
	printf("Stopping the motors..");
	roboclaw_duty_m1m2(rc, address, 0, 0);
	if(roboclaw_close(rc) != ROBOCLAW_OK) perror("Unable to shutdown roboclaw cleanly\n");
	else printf("Clean exit\n");
}

void* readEncoder (void *rc_)
{
	roboclaw *rc = (roboclaw *) rc_;
	int enc1, enc2;
	while (keepRunning) {
		roboclaw_encoders(rc, address, &enc1, &enc2);
		printf("Encoder: %d\n", enc1);
		sleep(1);
	}
}

int main(int argc, char **argv)
{
	struct roboclaw *rc;
	pthread_t tid1, tid2;

	// initialize everything
	rc=roboclaw_init("/dev/serial0", baudrate);
	if( rc == NULL) {
		perror("Unable to initialize roboclaw");
		exit(1);
	}
	printf("Initialized communication with roboclaw\n");

	// multithreading
	pthread_create (&tid1, NULL, changePWM, rc);
	pthread_create (&tid2, NULL, readEncoder, rc);
	pthread_exit(NULL);
	return 0;
}
