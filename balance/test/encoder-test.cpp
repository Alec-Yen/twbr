#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "encoder.h"
#include "twbr.h"
#include "PiMotor.h"


int p1 = 18; //Motor 0 PWM (refers to BCM numbers "GPIO 18", not the physical pins)
int d1 = 23; //Motor 0 DIR
int p2 = 12;
int d2 = 16;

int e1a = 17;
int e1b = 27;
int e2a = 5;
int e2b = 6;

int main()
{
	//TWBR robot(p1,d1,p2,d2);
	//robot.writePWMSame(0,10);

	struct encoder *enc1 = setupencoder(e1a,e1b);
	struct encoder *enc2 = setupencoder(e2a,e2b);

	printf("Turn the wheels to see how encoders respond\n");
	usleep(1000000);
	while(1) {
		updateEncoders();
		printf("enc1->value = %ld\tenc1->lastEncoded = %d\t", enc1->value,enc1->lastEncoded);
		printf("enc2->value = %ld\tenc2->lastEncoded = %d\n", enc2->value,enc2->lastEncoded);
	}

	return 0;
}



