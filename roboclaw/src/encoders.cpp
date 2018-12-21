#include "roboclaw.h"

#include <stdio.h>  //printf
#include <stdlib.h> //exit
#include <unistd.h> //sleep


int main(int argc, char **argv)
{
	struct roboclaw *rc;
	uint8_t address=0x80; //address of roboclaw unit
	int baudrate=38400;

	int duty_cycle;
	int enc1, enc2;
	
	rc=roboclaw_init("/dev/serial0", baudrate);

	if( rc == NULL )
	{
		perror("unable to initialize roboclaw");
		exit(1);
	}

	printf("initialized communication with roboclaw\n");
	
	//read the battery voltage
/*	int16_t voltage;
	if(roboclaw_main_battery_voltage(rc, address, &voltage) != ROBOCLAW_OK)
		informative_terminate(rc);
*/
/*float voltage_float = (float)voltage/10.0f;
	printf("battery voltage is : %f V\n", voltage_float);
	printf("WARNING - make sure it is safe for the motors to be moved\n\n");
*/

	while(1) {
//		printf("enter duty cycle [-100, 100] or 'q'  to quit\n");
		if( scanf("%d", &duty_cycle) < 1 ) break;
		if( duty_cycle > 100 ) duty_cycle = 100;
		if( duty_cycle < -100 ) duty_cycle = -100;
		//32767 is max duty cycle setpoint that roboclaw accepts
		duty_cycle = (float)duty_cycle/100.0f * 32767;	
		
		if(roboclaw_duty_m1m2(rc, address, duty_cycle, duty_cycle) != ROBOCLAW_OK )
			{ fprintf(stderr, "problem communicating with roboclaw, terminating\n"); break;	}	

		roboclaw_encoders(rc, address, &enc1, &enc2);
		printf("%d\n", enc1);

	}
	
	//make sure the motors are stopped before leaving
	printf("stopping the motors..");
	roboclaw_duty_m1m2(rc, address, 0, 0);

	if(roboclaw_close(rc) != ROBOCLAW_OK) perror("unable to shutdown roboclaw cleanly\n");
	else printf("clean exit\n");

	return 0;
}
