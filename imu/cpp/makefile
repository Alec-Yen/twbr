BIN = bin/

all: MPU6050_example_1

clean:
	rm -f *.o $(BIN)MPU6050_example_1

MPU6050_example_1: MPU6050_example_1.cpp
	gcc -o $(BIN)MPU6050_example_1 MPU6050_example_1.cpp \
		-I RaspberryPi_bcm2835/I2Cdev RaspberryPi_bcm2835/I2Cdev/I2Cdev.cpp \
		-I Arduino_MPU6050 Arduino_MPU6050/MPU6050.cpp -l bcm2835 -l m
