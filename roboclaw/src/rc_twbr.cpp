#include "rc_twbr.hpp"

// constructor
RClaw::RClaw(string tty_, int baudrate_, int address_)
{
	rc = roboclaw_init (tty_.c_str(), baudrate_);
	if (rc==NULL) fprintf(stderr,"Unable to initialize roboclaw\n");
	else printf("Initialized roboclaw successfully\n");
	tty = tty_;
	baudrate = baudrate_;
	address = address_;
}

// destructor
RClaw::~RClaw()
{
	moveSame (0,1000); // stop the motors
	if (roboclaw_close (rc) != ROBOCLAW_OK) // close safely
		fprintf(stderr,"Unable to shutdown roboclaw cleanly\n");
	else printf("Shutdown roboclaw successfully\n");
}

// if time_ms equals -1, indefinite
int RClaw::writePWMDuration (double pwm1, double pwm2, int time_ms)
{
	int status;
	int duty_cycle1 = pwm1/100.0f * 32767;
	int duty_cycle2 = pwm2/100.0f * 32767;

	// error handling
	if (pwm1>100 || pwm1<-100 || pwm2>100 || pwm2<-100)
		fprintf(stderr,"writePWMDuration Usage:\n set pwm as duty cycle from -100 to 100\n");
	if (!(time_ms == -1 || time_ms > 0))
		fprintf(stderr,"writePWMDuration Usage:\nset set time_ms in milliseconds, set to -1 for indefinite\n");

	// set duty cycle
	status = roboclaw_duty_m1m2 (rc, address, duty_cycle1, duty_cycle2);
	if (status != ROBOCLAW_OK)
		fprintf(stderr,"roboclaw_duty_m1m2 Error:\nProblem communicating with roboclaw, terminating\n");
	
	// stop if definite time duration
	if (time_ms != -1) {
		usleep (time_ms*1000);
		roboclaw_duty_m1m2 (rc, address, 0, 0);
	}
	return status;
}


int RClaw::readEncoders (int &enc1, int &enc2)
{
	int status;
	status = roboclaw_encoders (rc, address, &enc1, &enc2);
	if (status != ROBOCLAW_OK)
		fprintf(stderr,"roboclaw_encoders Error:\nProblem communicating with roboclaw, terminating\n");
	return status;
}





// to be deprecated
int RClaw::moveSame (double pwm, int ms)
{
	int status;
	int duty_cycle = pwm/100.0f * 32767;
	status = roboclaw_duty_m1m2 (rc, address, duty_cycle, duty_cycle);
	if (status != ROBOCLAW_OK)
		fprintf(stderr,"Problem communicating with roboclaw, terminating\n");
	usleep (ms*1000);
	roboclaw_duty_m1m2 (rc, address, 0, 0);
	return status;
}
