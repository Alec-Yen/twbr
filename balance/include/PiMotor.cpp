#include "PiMotor.h"

// uses wiringPi library


PiMotor::PiMotor()
{
}

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

    pinMode(pwmPin, PWM_OUTPUT); // set GPIO pin as output
		pinMode(dirPin, OUTPUT);
    pwmWrite(pwmPin, 0);
		digitalWrite(dirPin, 0); //FIX: potential bug?
    
    if (DEBUG) {
        printf("Stopping motors on pin %i.\n\r", pwmPin);
    }
   
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

   //Set motor as output.
    pinMode(pwmPin, PWM_OUTPUT);
    pinMode(dirPin, OUTPUT);

    pwmWrite(pwmPin, speed);
    digitalWrite(dirPin, direction); // FIX: will need to fix this depending if left or right

   
   //Free resources & GPIO access
//   gpioTerminate();
}
