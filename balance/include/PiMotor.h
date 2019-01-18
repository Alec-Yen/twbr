#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>

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

class TWBR {
	protected:
		PiMotor lMotor;
		PiMotor rMotor;
	public:
		TWBR(int,int,int,int);
		void writePWMSame (int FB,int speed);
		void moveSame(int FB, int speed, double ms);
		void wait(double ms);
		void stop();
};



#endif /* PIMOTOR_H */
