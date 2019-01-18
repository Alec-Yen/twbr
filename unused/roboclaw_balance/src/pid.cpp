#include "pid.h"

double SPid::UpdatePID(double error, double position)
{
	double pTerm, dTerm, iTerm;
	pTerm = propGain * error; // calculate the proportional term
	// calculate the integral state with appropriate limiting
	integratState += error;
	// Limit the integrator state if necessary
	if (integratState > integratMax)
	{
		integratState = integratMax;
	}
	else if (integratState < integratMin)
	{
		integratState = integratMin;
	}
	// calculate the integral term
	iTerm = integratGain * integratState;
	// calculate the derivative
	dTerm = derGain * (derState - position);
	derState = position;
	return pTerm + dTerm + iTerm;
}
