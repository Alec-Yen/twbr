# Main Directory 

## Overview  
This repository includes code to balance using MDD10A motor driver and MPU6050 IMU. Actively being developed.    
Libraries: bcm2835, wiringPi  

doc/ - all our documentation for wiring, etc  
include/ - class headers and member functions  
src/ - primary programs  
test/ - testing programs  


## Getting Started  
Run the makefile, and execute `sudo ./bin/balance_timing 32 0 .4 0 0` to balance robot without battery at 12V
To stop, type "q" and press ENTER

## Troubleshooting
PID constants are incredibly sensitive and specific. A difference between 30 and 35 for Kp, for example, makes a huge difference.
The robot needs to be perfectly balanced. The slightest imbalance (due to a hanging cord, for example) will cause it to drift.


## Notes   
balance_timing.cpp - more precise timing, denoising imu measurements  
no battery: `sudo ./bin/balance_timing 32 0 .4 0 0`  
battery: TBD  


balance_no_thread.cpp - does not handle interrupts, is old code   
balance.cpp - uses c++11 threading, will be replaced  
balance_pthread.cpp - uses c++98 threading, is the future
