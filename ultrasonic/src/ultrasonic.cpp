#include <unistd.h>
#include <stdio.h>
#include <pigpio.h>

#define SONAR_TRIGGER 7
#define SONAR_ECHO    8

/* forward prototypes */
void sonarTrigger(void);
void sonarEcho(int gpio, int level, uint32_t tick);

int main()
{
	if (gpioInitialise()<0) return 1;

	gpioSetMode(SONAR_TRIGGER, PI_OUTPUT);
	gpioWrite  (SONAR_TRIGGER, PI_OFF);
	gpioSetMode(SONAR_ECHO,    PI_INPUT);

	/* update sonar 20 times a second, timer #0 */
	gpioSetTimerFunc(0, 50, sonarTrigger); /* every 50ms */

	/* monitor sonar echos */
	gpioSetAlertFunc(SONAR_ECHO, sonarEcho);
	while (1) sleep(1);

	gpioTerminate();
	return 0;
}

void sonarTrigger(void)
{
	/* trigger a sonar reading */
	gpioWrite(SONAR_TRIGGER, PI_ON);
	gpioDelay(10); /* 10us trigger pulse */
	gpioWrite(SONAR_TRIGGER, PI_OFF);
}

void sonarEcho(int gpio, int level, uint32_t tick)
{
	// tick is the number of microseconds since boot
	// wraps  around from 4294967295 to 0, roughly every 72 minutes

	static uint32_t startTick, firstTick=0;
	int diffTick;

	if (!firstTick) firstTick = tick;
	if (level == PI_ON) startTick = tick;
	else if (level == PI_OFF)
	{
		diffTick = tick - startTick;
		printf("%d us\t\t%.2f cm\n", diffTick,(double)diffTick*0.034/2);
	}
}
