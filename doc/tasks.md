# TODO

## NOW
Write code to move forward and turn - balance_roboclaw/src/twbr.cpp 
Write code using PID advice - balance_roboclaw/src/pid.cpp
	https://www.embeddedrelated.com/showarticle/943.php


3D print chassis  
	Secure IMU so it is as close to aligned as possible  
		Get precise starting angle  
	Add training wheels or similar  
	3D print new mounting to put Pi and motor driver on middle layer  


## LATER:
Physically test balance_pthread code using MDD10A  
Physically test balance code using Roboclaw  
Write code to estimate position using Roboclaw encoder count (1920 counts per revolution)
Test MPU6050 code using libi2c-dev



## DEAN'S SUGGESTIONS
Increase max speed  
Use encoder to account for varying surfaces  
Eventually control motors separately  
