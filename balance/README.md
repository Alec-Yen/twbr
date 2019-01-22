# Main Directory   
  
## Overview    
This repository includes code to balance using MDD10A motor driver and MPU6050 IMU. Actively being developed.      
Libraries: bcm2835, wiringPi    
  
doc/ - all our documentation for wiring, etc    
include/ - class headers and member functions    
src/ - primary programs    
test/ - testing programs    
  
  
## Getting Started    
Run the makefile  
Hold robot upright with battery at 12V  
Execute `sudo ./bin/balance_encoder 55 6 55 .1 .8 0 0`  
To stop, type "q" and press ENTER  
  
## Troubleshooting  
When in doubt, reboot the Pi. `sudo shutdown -h now`  
If IMU readings get weird, unplug and reinstall the MPU6050.  
PID constants are incredibly sensitive and specific. A difference between 30 and 35 for Kp, for example, makes a huge difference.  
The robot needs to be perfectly balanced. The slightest imbalance (due to a hanging cord, for example) can cause it to drift.  
  
  
## Notes     
balance_encoder.cpp - best version, uses encoder and imu input  
  
balance_timing.cpp - uses imu input    
balance_pthread.cpp - uses c++98 threading    
balance_thread11.cpp - uses c++11 threading, will be replaced    
balance_no_thread.cpp - does not handle interrupts, is old code     
