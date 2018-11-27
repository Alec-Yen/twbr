/* 
 * File:   PiMotor.cpp
 * Author: Steve McMillan
 *
 * Created on 07 July 2017, 17:13
 */
#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>
#include "PiMotor.h"

PiMotor::PiMotor(int forwardPin, int reversePin) {
    if (DEBUG) {
        printf("Creating motor object with pins %i and %i\n\r", forwardPin, reversePin);
    }
    
    pwmPin = forwardPin;
    dirPin = reversePin;
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
    gpioSetMode(pwmPin, PI_OUTPUT);
    gpioSetMode(dirPin, PI_OUTPUT);
    gpioPWM(pwmPin, 0);
    gpioPWM(dirPin, 0);
    
    if (DEBUG) {
        printf("Stopping motors on pin %i and pin %i.\n\r", dirPin, pwmPin);
    }
   
   //Free resources & GPIO access
   gpioTerminate();   
}

void PiMotor::runForMS(int direction, int speed, int milliseconds) {
    //Convert Milliseconds to Microseconds for usleep (unix)
    unsigned int microseconds = (milliseconds * 1000.0);
    if (DEBUG) {
        printf("Running PiMotor::run for %i milliseconds.\n\r", milliseconds);
    }
    //Run
    PiMotor::run(direction, speed);
    //Wait
    usleep(microseconds);
    //Stop
    PiMotor::run(direction, 0);
    
    if (DEBUG) {
        printf("Stopping PiMotor::run\n\r");
    }
}

void PiMotor::run (int speed) {
    //Initialise GPIO.
    if (gpioInitialise() < 0) {
        if (DEBUG) {
            fprintf(stderr, "PiGPIO initialisation failed in PiMotor::run.\n\r");
        }
      return;
   }
  
  
   //Set motor as output.
    gpioSetMode(direction, PI_OUTPUT);
    gpioPWM(direction, speed);
    if (DEBUG) {
        printf("Setting speed to %i on motor pin %i \n\r", speed, direction);
    }
   
   //Free resources & GPIO access
   gpioTerminate();
}
