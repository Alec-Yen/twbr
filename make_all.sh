#!/bin/bash

echo '1. CHANGE DIRECTORY to balance_mdd10a' && \
cd balance_mdd10a && make && \
echo '2. CHANGE DIRECTORY to balance_roboclaw' && \
cd ../balance_roboclaw && make && \
echo '3. CHANGE DIRECTORY to mpu6050_bcm2835' && \
cd ../mpu6050_bcm2835 && make && \
echo '4. CHANGE DIRECTORY to mpu6050_nd' && \
cd ../mpu6050_nd && make && \
echo '5. CHANGE DIRECTORY to mdd10a' && \
cd ../mdd10a && make && \
echo '6. CHANGE DIRECTORY to roboclaw' && \
cd ../roboclaw && make
