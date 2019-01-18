# Balancing using Roboclaw

## Overview  
This repository includes code to balance using ROBOCLAW motor controller and MPU6050 IMU.This is the repo we want to use to balance.  
Libraries: bcm2835  
Dependencies: $(BASEDIR)/roboclaw, $(BASEDIR)/mpu6050  


## Getting Started  
```
make
sudo ./bin/balance 60 .5 .5 0 0
```
To stop, type "q" and press ENTER  

## Troubleshooting

## Notes  
balance.cpp - balances robot  
roam.cpp - enter 'f' and 'b' to move robot forward and backward, 'q' to break out  

