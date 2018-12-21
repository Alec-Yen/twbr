SRC = src
BIN = bin
OBJ = obj
INC = include

CC = gcc
CXX = g++ -std=c++11
DEBUG = -g
CFLAGS = -Wall -Wextra -c $(DEBUG)
CXX_FLAGS = -Wall -Wextra -c $(DEBUG)
LFLAGS =  -Wall -Wextra $(DEBUG)
LIBS = -lpigpio -lrt -lpthread -l bcm2835 -l m

BASEDIR = ..

BIN_FILES = $(BIN)/balance $(BIN)/stop
IMU_DIR = $(BASEDIR)/imu/cpp
MOTOR_DIR = $(BASEDIR)/motors
	

IMU_INC =  -I  $(IMU_DIR)/Arduino_MPU6050 $(IMU_DIR)/Arduino_MPU6050/MPU6050.cpp -I $(IMU_DIR)/RaspberryPi_bcm2835/I2Cdev $(IMU_DIR)/RaspberryPi_bcm2835/I2Cdev/I2Cdev.cpp
MOTOR_INC = -I $(MOTOR_DIR) $(MOTOR_DIR)/PiMotor.cpp


all: $(BIN_FILES)
balance: $(BIN)/balance
stop: $(BIN)/stop


$(BIN)/balance: $(SRC)/balance.cpp
	$(CXX) $(LFLAGS) -o $(BIN)/balance $(SRC)/balance.cpp \
	$(MOTOR_INC) $(IMU_INC) $(LIBS)

$(BIN)/stop: $(SRC)/stop.cpp
	$(CXX) $(LFLAGS) -o $(BIN)/stop $(SRC)/stop.cpp \
	$(MOTOR_INC) $(LIBS)



clean:
	rm -f $(OBJ)/*.o $(BIN_FILES)
