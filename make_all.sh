#!/bin/bash

echo 'Change directory to balance_mdd10a' && \
cd balance_mdd10a && make && \
echo 'Change directory to balance_roboclaw' && \
cd ../balance_roboclaw && make && \
echo 'Change directory to mpu6050_bcm2835' && \
cd ../mpu6050_bcm2835 && make && \
echo 'Change directory to mpu6050_nd' && \
cd ../mpu6050_nd && make && \
echo 'Change directory to mdd10a' && \
cd ../mdd10a && make && \
echo 'Change directory to roboclaw' && \
cd ../roboclaw && make
