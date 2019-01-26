# Main Directory   
  
## Overview    
This repository includes code to balance using MDD10A motor driver and MPU6050 IMU. Actively being developed.      
Libraries: bcm2835, wiringPi, PIGPIO    
  
doc/ - all our documentation for wiring, etc    
include/ - class headers and member functions    
src/ - primary programs    
test/ - testing programs    
  
  
## Getting Started    
Run the makefile  
Hold robot upright with battery at 12V  
Execute `sudo ./bin/balance_final` to view usage
Run the recommended script
To stop, type "q" and press ENTER  
  

## Troubleshooting  

### Software Development
Pins refer to BCM numbers "GPIO 18", not the physical pins, because we use wiringPiSetupGPIO() and (until we replace it with wiringPi) PIGPIO

### MPU6050 Misbehavior
When in doubt, reboot the Pi. `sudo shutdown -h now`  
If IMU readings get weird, unplug and reinstall the MPU6050.  

### PID Tuning
PID constants are incredibly sensitive and specific. A difference between 30 and 35 for Kp, for example, makes a huge difference.  
The robot needs to be perfectly balanced. The slightest imbalance (due to a hanging cord, for example) can cause it to drift.  
Changes in the structure/mass of the robot make significant differences, including battery placement and battery weight.  
  
  
## Notes     
src/  
balance_final.cpp - best version, adds encoders, removes calibration  
  
test/ *  
balance_calibration.cpp - adds calibration, averaging imu input
balance_timing.cpp - adds precise timing    
01/09/19: balance_pthread.cpp - replaces with c++98 threading    
12/07/19: balance_thread11.cpp - adds c++11 threading, adds interrupts     
12/04/19: balance_no_thread.cpp - first attempt at balancing  

*WARNING: the angles are calculated using the red Sparkfun MPU6050's orientation, not with the cheaper, Amazon blue MPU6050  
