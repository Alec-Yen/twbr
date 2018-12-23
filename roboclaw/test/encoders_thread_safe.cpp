#include "rc_twbr.hpp"
#include <stdio.h>  //printf
#include <stdlib.h> //exit
#include <unistd.h> //sleep
#include <pthread.h>

static bool keepRunning = true;
pthread_mutex_t lock;


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

			pthread_mutex_lock (&lock);
			robot->moveSame (duty_cycle,1000); // 1000 milliseconds
			pthread_mutex_unlock (&lock);
		}
	}
	pthread_mutex_lock (&lock);
	delete robot;
	pthread_mutex_unlock (&lock);

	return NULL;
}


void* readEncoder (void *robot_)
{
	RClaw *robot = (RClaw *) robot_;
	int enc1, enc2;
	while (keepRunning) {
		
		pthread_mutex_lock (&lock);
		robot->readEncoders(enc1,enc2);
		pthread_mutex_unlock( &lock);

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
	pthread_mutex_init (&lock, NULL);
	pthread_create (&tid1, NULL, changePWM, robot);
	pthread_create (&tid2, NULL, readEncoder, robot);
	pthread_join (tid1, NULL);
	pthread_join (tid2, NULL);
	printf("Threads joined successfully\n");
	return 0;
}
