# Balancing with MDD10A

## Overview  
This repository includes code to balance using MDD10A motor driver and MPU6050 IMU. This is an older repo that we are moving away from.  
Libraries: bcm2835, pigpio  
Dependencies: $(BASEDIR)/mdd10a, $(BASEDIR)/mpu6050  


## Getting Started  
Run the makefile, and execute `sudo ./bin/balance_timing 32 0 .4 0 0`
To stop, type "q" and press ENTER

## Troubleshooting
PID constants are incredibly sensitive and specific. A difference between 30 and 35 for Kp, for example, makes a huge difference.
The robot needs to be perfectly balanced. The slightest imbalance will cause it to drift.

## Notes   
balance_timing.cpp - more precise timing, denoising imu measurements  
no battery: `sudo ./bin/balance_timing 32 0 .4 0 0`  
battery: TBD  



balance_no_thread.cpp - does not handle interrupts, is old code   
balance.cpp - uses c++11 threading, will be replaced  
balance_pthread.cpp - uses c++98 threading, is the future
