#ifndef TWBR_H
#define TWBR_H

#include <stdint.h> //uint8_t, int16_t
#include <unistd.h>
#include <stdio.h>
#include <string>
#include "MPU6050.h"
#include "rc_twbr.hpp"

using namespace std;

class TWBR {
	public:
		// implemented member functions
		
		// not implemented member functions
		TWBR(string tty_, int baudrate_, uint8_t address_);
		~TWBR();
		int Standstill();
		int StandstillDuration(double ms_time);
		int MoveDuration(double pwm1, double pwm2, double ms_time);
		int MoveDistance(double pwm1, double pwm2, double distance);
		int TurnSpin(double angle);
		int TurnPivot(double angle);
		int TurnSmooth(double angle);


	protected:
		// member variables
		RClaw *rc;
		MPU6050 *mpu;
		string tty;
		int baudrate;
		uint8_t address;
};


#endif // TWBR_H
