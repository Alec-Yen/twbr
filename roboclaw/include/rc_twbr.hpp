#ifndef RC_TWBR_HPP
#define RC_TWBR_HPP

#include <stdint.h> //uint8_t, int16_t
#include <unistd.h>
#include <stdio.h>
#include <string>
#include "roboclaw.h"

using namespace std;

class RClaw {
	public:
		// member functions (future-proof)
		RClaw(string tty_, int baudrate_, uint8_t address_);
		~RClaw();
		int writePWM (double pwm1, double pwm2, double ms);
		void readEncoders (int &enc1, int &enc2);

		// member functions (to be deprecated)
		void moveSame (int pwm, double ms); // pwm: -100 to 100, ms: milliseconds

	protected:
		// member variables
		roboclaw *rc;
		string tty;
		int baudrate;
		uint8_t address;
};


#endif // RC_TWBR_HPP
