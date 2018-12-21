BIN = bin/

all: MPU6050_example_1

clean:
	rm -f *.o $(BIN)MPU6050_example_1

MPU6050_example_1: MPU6050_example_1.cpp
	gcc -o $(BIN)MPU6050_example_1 MPU6050_example_1.cpp \
		-I. I2Cdev.cpp \
		-I. MPU6050.cpp -l bcm2835 -l m
