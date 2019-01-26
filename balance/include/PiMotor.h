#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <pigpio.h>

#ifndef PIMOTOR_H
#define PIMOTOR_H


class PiMotor {
	protected:
		int motorID; // either 0 (for the left) or 1 (for the right)
		int pwmPin;
		int dirPin;
		bool DEBUG;
	public:
		PiMotor();
		PiMotor(int,int,int);
		void run (int,int);
		void runForMS(int,int,double ms);
		void stop();
		void setDebug(bool); 
};



#endif /* PIMOTOR_H */
