#include "TWAE.hpp"
#include <fstream>
#include <cmath>
#include <sstream>
#include <map>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>

using namespace std;
using namespace NeuroUtils;
//const string TWAE::PARAMS_STEM = "twowheel";

class TWAERun {
	public:
		TW_RunStatic S;
		TW_RunDynamic D;
};


TWAE::TWAE()
{
	TW_Static *s;
	s = new TW_Static;
	s->W = ParamsGetInt("twowheel","w",true);
	state = (void *) s;
}

TWAE::~TWAE()
{
	TW_Static *s;

	s = (TW_Static *) state;
	delete s;
}

void *TWAE::NewRun()
{
	TWAERun *ap;
	TW_Static *as;
	int i;
	unsigned int a;

	as = (TW_Static *) state;
	ap = new TWAERun;

}
