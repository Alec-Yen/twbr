#include "rc_twbr.hpp"
#include <stdio.h>  //printf
#include <stdlib.h> //exit
#include <unistd.h> //sleep
#include <pthread.h>

static bool keepRunning = true;

void* changePWM (void *robot_)
{
	RClaw *robot = (RClaw *) robot_;
	int duty_cycle;
	
	while(keepRunning) {
		if (scanf("%d",&duty_cycle) == 1) {
			if (duty_cycle == 0) {
				keepRunning = false;
			}

			if (duty_cycle > 100) duty_cycle = 100;
			if (duty_cycle < -100) duty_cycle = -100;

			robot->moveSame (duty_cycle,1000); // 1000 milliseconds
		}
	}
	delete robot;
	return NULL;
}


void* readEncoder (void *robot_)
{
	RClaw *robot = (RClaw *) robot_;
	int enc1, enc2;
	while (keepRunning) {
		robot->readEncoders(enc1,enc2);
		printf("Encoder: %d\n", enc1);
		sleep(1);
	}

	return NULL;
}


int main(int argc, char **argv)
{
	
	uint8_t address=0x80; //address of roboclaw unit
	int baudrate=38400;
	string tty = "/dev/serial0";
	
	RClaw *robot = new RClaw (tty, baudrate, address);
	pthread_t tid1, tid2;

	// multithreading
	pthread_create (&tid1, NULL, changePWM, robot);
	pthread_create (&tid2, NULL, readEncoder, robot);
	pthread_exit(NULL);
	return 0;
}
