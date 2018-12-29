#!/bin/bash

cd balance_mdd10a && make clean && \
cd ../balance_roboclaw && make clean && \
cd ../mpu6050_bcm2835 && make clean && \
cd ../mpu6050_i2c && make clean && \
cd ../mdd10a && make clean && \
cd ../roboclaw && make clean
