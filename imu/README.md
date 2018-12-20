This directory includes cpp and python code. We aren't currently using any of the code. See balance directory for application of imu code. Edited: 12/20/18


Issues: <br / >
Needing to use sudo commands on pi for some reason. <br />


Cpp Code: https://github.com/jrowberg/i2cdevlib <br />
Cpp Dependencies: bcm2835


Instructions for getting cpp code to work: <br />
1. Install bcm2835 library http://www.airspayce.com/mikem/bcm2835/index.html <br />
2. Enable I2C and connect IMU (see pinout in wiring) <br />
3. Use the included cpp/makefile
