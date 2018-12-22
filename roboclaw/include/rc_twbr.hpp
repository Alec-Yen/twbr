#ifndef RC_TWBR_HPP
#define RC_TWBR_HPP

#include <stdint.h> //uint8_t, int16_t
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include "roboclaw.h"

using namespace std;

class TWBR {
	public:
		// member functions
		TWBR(string tty_, int baudrate_, uint8_t address_);
		~TWBR();
		void moveSame (int pwm, double ms); // pwm: -255 to 255, ms: milliseconds
		void readEncoders (int &enc1, int &enc2);

		// member variables
		roboclaw *rc;
		string tty;
		int baudrate;
		uint8_t address;
};


#endif // RC_TWBR_HPP
