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

BIN_FILES = $(BIN)/balance $(BIN)/balance_pthread $(BIN)/stop
IMU_DIR = $(BASEDIR)/mpu6050
MOTOR_DIR = $(BASEDIR)/mdd10a
	

IMU_INC =  -I$(IMU_DIR) $(IMU_DIR)/MPU6050.cpp -I$(IMU_DIR) $(IMU_DIR)/I2Cdev.cpp
MOTOR_INC = -I $(MOTOR_DIR) $(MOTOR_DIR)/PiMotor.cpp


all: $(BIN_FILES)
balance: $(BIN)/balance
stop: $(BIN)/stop


$(BIN)/balance_pthread: $(SRC)/balance_pthread.cpp
	$(CXX) $(LFLAGS) -o $(BIN)/balance_pthread $(SRC)/balance_pthread.cpp \
	$(MOTOR_INC) $(IMU_INC) $(LIBS)


$(BIN)/balance: $(SRC)/balance.cpp
	g++ -std=c++11 $(LFLAGS) -o $(BIN)/balance $(SRC)/balance.cpp \
	$(MOTOR_INC) $(IMU_INC) $(LIBS)

$(BIN)/stop: $(SRC)/stop.cpp
	$(CXX) $(LFLAGS) -o $(BIN)/stop $(SRC)/stop.cpp \
	$(MOTOR_INC) $(LIBS)



clean:
	rm -f $(OBJ)/*.o $(BIN_FILES)
