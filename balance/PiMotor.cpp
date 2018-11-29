#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>
#include "PiMotor.h"


TWBR::TWBR(int p1, int d1, int p2, int d2) {
	lMotor = PiMotor(0,p1,d1);
	rMotor = PiMotor(1,p2,d2);
}

// FB: 0 for forward, 1 for reverse
// speed 0 to 255
void TWBR::moveSame (int FB,int speed, double ms)
{
	int ld, rd;
	if (FB) { ld = 0; rd = 1; }
	else { ld = 1; rd = 0; }

	lMotor.run(ld, speed);
	rMotor.run(rd, speed);

  usleep(ms*1000.0); //in microseconds

	lMotor.run(ld, 0);
	rMotor.run(rd, 0);
}

void TWBR::wait (double ms)
{
	usleep (ms*1000.0);
}

PiMotor::PiMotor() {}

PiMotor::PiMotor(int motorID_, int pwmPin_, int dirPin_) {
    if (DEBUG) {
        printf("Creating motor object with pins %i and %i\n\r", pwmPin_, dirPin_);
    }
    
    motorID = motorID_;
    pwmPin = pwmPin_;
    dirPin = dirPin_;
}

void PiMotor::setDebug(bool debug) {
    DEBUG = debug;
    if (DEBUG) {
        printf("Debug messages enabled. Outputting all operations...\n\r");
    }
}

void PiMotor::stop() {
    //Initialise GPIO.
    if (gpioInitialise() < 0) {
        if (DEBUG) {
            fprintf(stderr, "PiGPIO initialisation failed.\n\r");
        }
      return;
   }
    gpioSetMode(pwmPin, PI_OUTPUT); // set GPIO pin as output
		gpioSetMode(dirPin, PI_OUTPUT);
    gpioPWM(pwmPin, 0);
		gpioWrite(dirPin, 0); //FIX: potential bug?
    
    if (DEBUG) {
        printf("Stopping motors on pin %i.\n\r", pwmPin);
    }
   
   //Free resources & GPIO access
   gpioTerminate();   
}

// direction = 0 (forward), 1 (backward)
void PiMotor::runForMS(int direction, int speed, double ms) {

		if (DEBUG) {
        printf("Setting speed to %d on motor pin %d to %f \n\r", speed, pwmPin, ms*1000.0);
    }

    //Run
    PiMotor::run(direction, speed);
    //Wait
    usleep(ms*1000.0); //in microseconds
    //Stop
    PiMotor::run(direction, 0);
    
    if (DEBUG) {
        printf("Stopping PiMotor::run\n\r");
    }
}

// speed is from 0 to 255
void PiMotor::run (int direction, int speed) {

	//Initialise GPIO.
    if (gpioInitialise() < 0) {
        if (DEBUG) {
            fprintf(stderr, "PiGPIO initialisation failed in PiMotor::run.\n\r");
        }
      return;
	  }
  
   //Set motor as output.
    gpioSetMode(pwmPin, PI_OUTPUT);
    gpioSetMode(dirPin, PI_OUTPUT);

    gpioPWM(pwmPin, speed);
    gpioWrite(dirPin, direction); // FIX: will need to fix this depending if left or right

   
   //Free resources & GPIO access
//   gpioTerminate();
}
