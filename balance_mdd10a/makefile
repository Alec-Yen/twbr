SRC = src
BIN = bin
OBJ = obj
INC = include

CC = gcc
CXX = g++ -std=c++98
DEBUG = -g
CFLAGS = -Wall -Wextra -c $(DEBUG) -lpthread
CXX_FLAGS = -Wall -Wextra -c $(DEBUG) -lpthread
LFLAGS =  -Wall -Wextra $(DEBUG) -lpthread
LIBS = -lpigpio -lrt -l bcm2835 -l m


BASEDIR = ..
IMU_DIR = $(BASEDIR)/mpu6050
MOTOR_DIR = $(BASEDIR)/mdd10a

IMU_INC =  -I$(IMU_DIR)/$(INC) $(IMU_DIR)/$(SRC)/MPU6050.cpp $(IMU_DIR)/$(SRC)/I2Cdev.cpp
MOTOR_INC = -I$(MOTOR_DIR)/$(INC) $(MOTOR_DIR)/$(SRC)/PiMotor.cpp

BIN_FILES = $(BIN)/balance $(BIN)/balance_pthread $(BIN)/stop

all: $(BIN_FILES)

$(BIN)/balance_pthread: $(SRC)/balance_pthread.cpp
	$(CXX) $(LFLAGS) -o $@ $< \
	$(MOTOR_INC) $(IMU_INC) $(LIBS)

$(BIN)/balance: $(SRC)/balance.cpp
	g++ -std=c++11 $(LFLAGS) -o $@ $< \
	$(MOTOR_INC) $(IMU_INC) $(LIBS)

$(BIN)/stop: $(SRC)/stop.cpp
	$(CXX) $(LFLAGS) -o $@ $< \
	$(MOTOR_INC) $(LIBS)




clean:
	rm -f $(OBJ)/*.o $(BIN_FILES)
