# Roboclaw Motor Controller

## Overview  
This repository includes code to operate ROBOCLAW motor controller using Pi.  
Libraries:  
Dependencies:  


## Getting Started  
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
To connect to Pi, see wiring/roboclaw images (use RX1 and TX1 on Pi)  
Cpp Code: https://github.com/bmegli/roboclaw   

