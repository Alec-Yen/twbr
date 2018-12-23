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
	int status;
	double duty_cycle, seconds;

	while(keepRunning) {
		if (scanf("%lf %lf",&duty_cycle,&seconds) == 2
				&& duty_cycle < 100 && duty_cycle > -100 && seconds >= 0)
		{
			if (duty_cycle == 0) {
				keepRunning = false;
				break;
			}

			pthread_mutex_lock (&lock);
			status = robot->writePWMDuration (duty_cycle,duty_cycle,seconds*1000);
			pthread_mutex_unlock (&lock);

			if (status != ROBOCLAW_OK) {
				keepRunning = false;
				break;
			}
		}
		else {
			printf("Usage:\n\t<PWM (-100 to 100)> <duration (seconds)>\n\tEnter 0 0 to break\n");
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
	
	printf("%s Usage:\n\t<PWM (-100 to 100)> <duration (seconds)>\n\tEnter 0 0 to break\n",argv[0]);

	int address=0x80; //address of roboclaw unit
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
