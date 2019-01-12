# Template README.md

## Overview  
This repository includes code to operate ROBOCLAW motor controller using Pi.  
Libraries:  
Dependencies:  


## Getting Started  
To connect to Pi, see wiring/roboclaw images (use RX1 and TX1 on Pi)  
```
make  
sudo ./bin/roboclaw-test /dev/serial0 38400 0x80
```


## Troubleshooting   
Raspberry Pi needs a common ground with RoboClaw   
Perform PID tuning using Basicmicro Motion Studio   
Reboot Pi  
Run `sudo ./bin/roboclaw-test /dev/serial0 38400 0x80`


## Notes   
roboclaw-test.c - simple test with pwm input to see if roboclaw is operational using roboclaw struct
encoders.cpp - simple test with pwm input and time duration using RClaw class
encoders_thread.cpp - multithreading
encoders_thread_safe.cpp - use mutex and locks
https://github.com/bmegli/roboclaw   

