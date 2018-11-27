This repository holds python and cpp code for the IMU. Will add schematics of pinout later.
simple_impu.py worked but has ugly looking output. Cpp example 1 also works nicely.

Needing to use sudo commands on pi for some reason.
The IMU appears extraordinarily sensitive.



Cpp Dependencies:
bcm2835

Instructions for getting cpp code to work:
1. Install bcm2835 library http://www.airspayce.com/mikem/bcm2835/index.html
2. Enable I2C and connect IMU
3. Use the included makefile
