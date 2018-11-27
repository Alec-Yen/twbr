from simple_imu import read_raw_data, MPU_Init
from time import sleep
from MDD10A import setMotorLeft
import  RPi.GPIO as GPIO
import time
import smbus

##Memory address for the IMU
PWR_MGMT_1   = 0x6B
SMPLRT_DIV   = 0x19
CONFIG       = 0x1A
GYRO_CONFIG  = 0x1B
INT_ENABLE   = 0x38
ACCEL_XOUT_H = 0x3B
ACCEL_YOUT_H = 0x3D
ACCEL_ZOUT_H = 0x3F
GYRO_XOUT_H  = 0x43
GYRO_YOUT_H  = 0x45
GYRO_ZOUT_H  = 0x47
##Defining values for Motors
Dir1 = 16 
Dir2 = 36


##This is the GPIO pin set up. I define that I want to output at the pins of 12 and 32 and that i want the output to be a PWM signal
GPIO.setmode(GPIO.BOARD)
GPIO.setup(12,GPIO.OUT)
GPIO.setup(32,GPIO.OUT)
GPIO.setup(Dir1,GPIO.OUT)
GPIO.setup(Dir2,GPIO.OUT)
GPIO.output(Dir1, False)
GPIO.output(Dir2, False)
pwm1=GPIO.PWM(12,100)
pwm2=GPIO.PWM(32,100)


bus = smbus.SMBus(1)
Device_Address = 0x68

MPU_Init()

setMotorLeft(50)
"""
while True:
	
	#Read Accelerometer raw value
	acc_x = read_raw_data(ACCEL_XOUT_H)
	acc_y = read_raw_data(ACCEL_YOUT_H)
	acc_z = read_raw_data(ACCEL_ZOUT_H)
	
	#Read Gyroscope raw value
	gyro_x = read_raw_data(GYRO_XOUT_H)
	gyro_y = read_raw_data(GYRO_YOUT_H)
	gyro_z = read_raw_data(GYRO_ZOUT_H)
	
	#Full scale range +/- 250 degree/C as per sensitivity scale factor
	Ax = acc_x/16384.0
	Ay = acc_y/16384.0
	Az = acc_z/16384.0
	
	Gx = gyro_x/131.0
	Gy = gyro_y/131.0
	Gz = gyro_z/131.0

        ## If the IMU is oriented in the negative x direction the motors will move forward, if in the negative direction, the motors will move in reverse
        if(Ax < 0):
	    acc_x = read_raw_data(ACCEL_XOUT_H)
            Ax = gyro_x/16384.0
            pwm1.start(10)
            pwm2.start(10)
        elif(Ax >= 0):
            pwm1.stop()
            pwm2.stop()

            """
