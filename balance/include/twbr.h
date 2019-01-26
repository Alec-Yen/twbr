#include "PiMotor.h"

#ifndef TWBR_H
#define TWBR_H

class TWBR {
	protected:
		PiMotor lMotor;
		PiMotor rMotor;
	public:
		// active methods
		TWBR(int,int,int,int);
		void writePWMSame (int FB,int speed);
		void wait(double ms);
		void stop();
		
		// deprecated methods
		void moveSame(int FB, int speed, double ms);
};

#endif
