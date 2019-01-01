typedef struct
{
	double derState;      // Last position input
	double integratState; // Integrator state
	double integratMax,   // Maximum and minimum
				 integratMin;   // allowable integrator state
	double integratGain,  // integral gain
				 propGain,      // proportional gain
				 derGain;       // derivative gain
} SPid;

