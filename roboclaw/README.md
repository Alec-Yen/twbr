# Roboclaw Motor Controller

Roboclaw commands are functional, still being cleaned up. Edited 12/21/2018   

Troubleshooting:   
Raspberry Pi needs a common ground with RoboClaw   
Perform PID tuning using Basicmicro Motion Studio   
Reboot Pi
Run `sudo ./bin/roboclaw-test /dev/serial0 38400 0x80`


To run:   
```
cd cpp 
make all  
sudo ./bin/roboclaw-test /dev/serial0 38400 0x80
```

Primary Cpp Code: https://github.com/bmegli/roboclaw   
