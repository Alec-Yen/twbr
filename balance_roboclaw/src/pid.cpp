#include "pid.h"

double UpdatePID(SPid * pid, double error, double position)
{
	double pTerm, dTerm, iTerm;
	pTerm = pid->propGain * error; // calculate the proportional term
	// calculate the integral state with appropriate limiting
	pid->integratState += error;
	// Limit the integrator state if necessary
	if (pid->integratState > pid->integratMax)
	{
		pid->integratState = pid->integratMax;
	}
	else if (pid->integratState < pid->integratMin)
	{
		pid->integratState = pid->integratMin;
	}
	// calculate the integral term
	iTerm = pid->integratGain * pid->integratState;
	// calculate the derivative
	dTerm = pid->derGain * (pid->derState - position);
	pid->derState = position;
	return pTerm + dTerm + iTerm;
}
