BIN = bin/
OBJ = obj/

all: main 

clean:
	rm -f *.o $(BIN)main

main: main.cpp
	g++ -o $(BIN)main main.cpp -I ../motordriver/PiMotor ../motordriver/PiMotor.cpp \
					-I  ../imu/cpp/Arduino_MPU6050 ../imu/cpp/Arduino_MPU6050/MPU6050.cpp \
	      	-I ../imu/cpp/RaspberryPi_bcm2835/I2Cdev ../imu/cpp/RaspberryPi_bcm2835/I2Cdev/I2Cdev.cpp \
					-lpigpio -lrt -lpthread -l bcm2835 -l m
