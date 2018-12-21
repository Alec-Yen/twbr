CPP = g++
BIN = bin
SRC = src
OBJ = obj
FLAGS = -std=c++11 -g -Wall -Wextra
LIBS = -lpigpio -lrt -lpthread -l bcm2835 -l m

BASEDIR = ..
IMU_DIR = $(BASEDIR)/imu/cpp
MOTOR_DIR = $(BASEDIR)/motors

IMU_INC =  -I  $(IMU_DIR)/Arduino_MPU6050 $(IMU_DIR)/Arduino_MPU6050/MPU6050.cpp -I $(IMU_DIR)/RaspberryPi_bcm2835/I2Cdev $(IMU_DIR)/RaspberryPi_bcm2835/I2Cdev/I2Cdev.cpp
MOTOR_INC = -I $(MOTOR_DIR) $(MOTOR_DIR)/PiMotor.cpp

all: $(BIN)/balance
balance: $(BIN)/balance
stop: $(BIN)/stop

clean:
	rm -f *.o

$(BIN)/balance: $(SRC)/balance.cpp
	$(CPP) $(FLAGS) -o $(BIN)/balance $(SRC)/balance.cpp $(MOTOR_INC) $(IMU_INC) $(LIBS)

$(BIN)/stop: $(SRC)/stop.cpp
	$(CPP) $(FLAGS) -o $(BIN)/stop $(SRC)/stop.cpp $(MOTOR_INC) $(LIBS)
