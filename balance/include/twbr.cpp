#include "twbr.h"


// calls setup function TODO maybe don't do that
TWBR::TWBR(int p1, int d1, int p2, int d2) {
	wiringPiSetupGpio();
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


void TWBR::writePWMSame (int FB,int speed)
{
	int ld, rd;
	if (FB) { ld = 0; rd = 1; }
	else { ld = 1; rd = 0; }

	lMotor.run(ld, speed);
	rMotor.run(rd, speed);
}



void TWBR::wait (double ms)
{
	usleep (ms*1000.0);
}

void TWBR::stop()
{
	lMotor.stop();
	rMotor.stop();
}

