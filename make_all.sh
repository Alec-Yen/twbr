#!/bin/bash

cd balance_mdd10a && make && \
cd ../balance_roboclaw && make && \
cd ../mpu6050_bcm2835 && make && \
cd ../mpu6050_nd && make && \
cd ../mdd10a && make && \
cd ../roboclaw && make
