## Roboclaw Motor Controller

Roboclaw commands are functional, still being cleaned up. Edited 12/21/2018 <br />

Troubleshooting: <br />
Raspberry Pi needs a common ground with RoboClaw <br />
Perform PID tuning using Basicmicro Motion Studio <br />
Reboot Pi
Run `sudo ./bin/roboclaw-test /dev/serial0 38400 0x80`


To run: <br />
`cd cpp` <br />
`make all` <br />
`sudo ./bin/roboclaw-test /dev/serial0 38400 0x80`

Primary Cpp Code: https://github.com/bmegli/roboclaw <br />
