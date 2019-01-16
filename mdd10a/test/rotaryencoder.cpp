#include <wiringPi.h>
#include <pigpio.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "PiMotor.h"

#define max_encoders 8

struct encoder
{
    int pin_a;
    int pin_b;
    volatile long value;
    volatile int lastEncoded;
};

//Pre-allocate encoder objects on the stack so we don't have to 
//worry about freeing them
struct encoder encoders[max_encoders];

/*
  Should be run for every rotary encoder you want to control
  Returns a pointer to the new rotary encoder structer
  The pointer will be NULL is the function failed for any reason
*/
struct encoder *setupencoder(int pin_a, int pin_b); 




int numberofencoders = 2;

void updateEncoders()
{
    struct encoder *encoder = encoders;
    for (; encoder < encoders + numberofencoders; encoder++)
    {
        int MSB = digitalRead(encoder->pin_a);
        int LSB = digitalRead(encoder->pin_b);

        int encoded = (MSB << 1) | LSB;
        int sum = (encoder->lastEncoded << 2) | encoded;

        if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoder->value++;
        if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoder->value--;

        encoder->lastEncoded = encoded;
    }
}

struct encoder *setupencoder(int pin_a, int pin_b)
{
    if (numberofencoders > max_encoders)
    {
        printf("Maximum number of encodered exceded: %i\n", max_encoders);
        return NULL;
    }

    struct encoder *newencoder = encoders + numberofencoders++;
    newencoder->pin_a = pin_a;
    newencoder->pin_b = pin_b;
    newencoder->value = 0;
    newencoder->lastEncoded = 0;

    pinMode(pin_a, INPUT);
    pinMode(pin_b, INPUT);
    pullUpDnControl(pin_a, PUD_UP);
    pullUpDnControl(pin_b, PUD_UP);
    wiringPiISR(pin_a,INT_EDGE_BOTH, updateEncoders);
    wiringPiISR(pin_b,INT_EDGE_BOTH, updateEncoders);

    return newencoder;
}


int p1 = 18; //Motor 0 PWM (refers to BCM numbers "GPIO 18", not the physical pins)
int d1 = 23; //Motor 0 DIR
int p2 = 12;
int d2 = 16;

int main()
{
	TWBR robot(p1,d1,p2,d2);
	robot.writePWMSame(0,40);

	wiringPiSetup();
	struct encoder *enc1 = setupencoder(17,27);
	struct encoder *enc2 = setupencoder(5,6);
	while(1) {
		updateEncoders();
		printf("enc1->value = %ld\tenc1->lastEncoded = %d\t", enc1->value,enc1->lastEncoded);
		printf("enc2->value = %ld\tenc2->lastEncoded = %d\n", enc2->value,enc2->lastEncoded);
	}

	return 0;
}



