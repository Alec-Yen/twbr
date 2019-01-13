#include <iostream>
#include <cstdio> // for printf
#include "timing.h"
using namespace std;

// for timing
int STD_LOOP_TIME = 9; // in milliseconds            
int lastLoopTime = STD_LOOP_TIME;
int lastLoopUsefulTime = STD_LOOP_TIME;
unsigned long loopStartTime = millis();


static int skip = 0;

int main()
{
	while (1) {
		// balancing code
		// blah blah blah

		// print out
		if (skip++==5) {
			skip = 0;
			printf("lastLoopUsefulTime=%10d, lastLoopTime=%10d\n",lastLoopUsefulTime,lastLoopTime);
		}

		// code to make sure that code is executed precisely every STD_LOOP_TIME 
		lastLoopUsefulTime = millis()-loopStartTime;
		if(lastLoopUsefulTime<STD_LOOP_TIME)			delay(STD_LOOP_TIME-lastLoopUsefulTime);
		lastLoopTime = millis() - loopStartTime;
		loopStartTime = millis();
	}
}
