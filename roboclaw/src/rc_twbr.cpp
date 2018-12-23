#include "rc_twbr.hpp"

RClaw::RClaw(string tty_, int baudrate_, uint8_t address_)
{
	rc = roboclaw_init (tty_.c_str(), baudrate_);
	if (rc==NULL) fprintf(stderr,"Unable to initialize roboclaw\n");
	else printf("Initialized roboclaw successfully\n");
	tty = tty_;
	baudrate = baudrate_;
	address = address_;
}

RClaw::~RClaw()
{
	moveSame (0,1000);
	if (roboclaw_close (rc) != ROBOCLAW_OK)
		fprintf(stderr,"Unable to shutdown roboclaw cleanly\n");
	else printf("Shutdown roboclaw successfully\n");
}

void RClaw::moveSame (int pwm, double ms)
{
	int duty_cycle = pwm/100.0f * 32767;
	if (roboclaw_duty_m1m2 (rc, address, duty_cycle, duty_cycle) != ROBOCLAW_OK)
		fprintf(stderr,"Problem communicating with roboclaw, terminating\n");
	usleep (ms*1000);
	roboclaw_duty_m1m2 (rc, address, 0, 0);
}

void RClaw::readEncoders (int &enc1, int &enc2)
{
	roboclaw_encoders (rc, address, &enc1, &enc2);
}
