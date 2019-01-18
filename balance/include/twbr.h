#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include "PiMotor.h"

#ifndef TWBR_H
#define TWBR_H

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

#endif
