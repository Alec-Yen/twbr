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
MOTOR_DIR = $(BASEDIR)/roboclaw

IMU_INC = $(IMU_DIR)/$(INC)/MPU6050.h $(IMU_DIR)/$(INC)/I2Cdev.h
MOTOR_INC = $(MOTOR_DIR)/$(INC)/roboclaw.h $(MOTOR_DIR)/$(INC)/rc_twbr.hpp

IMU_OBJ = $(IMU_DIR)/$(OBJ)/MPU6050.o $(IMU_DIR)/$(OBJ)/I2Cdev.o
MOTOR_OBJ = $(MOTOR_DIR)/$(OBJ)/roboclaw.o $(MOTOR_DIR)/$(OBJ)/rc_twbr.o

BIN_FILES = $(BIN)/balance $(BIN)/roam
OBJ_FILES = $(IMU_OBJ) $(MOTOR_OBJ)
all: $(BIN_FILES)



# balance bin
$(BIN)/balance: $(OBJ)/balance.o $(OBJ_FILES)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/roam: $(OBJ)/roam.o $(OBJ_FILES)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

# balance obj
$(OBJ)/%.o: $(TEST)/%.cpp
	$(CXX) $(CXX_FLAGS) -I$(INC) -I$(IMU_DIR)/$(INC) -I$(MOTOR_DIR)/$(INC) $< -o $@

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXX_FLAGS) -I$(INC) -I$(IMU_DIR)/$(INC) -I$(MOTOR_DIR)/$(INC) $< -o $@



# motor obj
$(MOTOR_DIR)/$(OBJ)/%.o: $(MOTOR_DIR)/$(TEST)/%.cpp $(MOTOR_INC) 
	$(CXX) $(CXX_FLAGS) -I$(MOTOR_DIR)/$(INC) $< -o $@ $(LIBS)

$(MOTOR_DIR)/$(OBJ)/%.o: $(MOTOR_DIR)/$(SRC)/%.cpp $(MOTOR_INC)
	$(CXX) $(CXX_FLAGS) -I$(MOTOR_DIR)/$(INC) $< -o $@ $(LIBS)

$(MOTOR_DIR)/$(OBJ)/%.o: $(MOTOR_DIR)/$(TEST)/%.c $(MOTOR_INC)
	$(CC) $(CFLAGS) -I$(MOTOR_DIR)/$(INC) $< -o $@ $(LIBS)

$(MOTOR_DIR)/$(OBJ)/%.o: $(MOTOR_DIR)/$(SRC)/%.c $(MOTOR_INC)
	$(CC) $(CFLAGS) -I$(MOTOR_DIR)/$(INC) $< -o $@ $(LIBS)

# imu obj
$(IMU_DIR)/$(OBJ)/%.o: $(IMU_DIR)/$(SRC)/%.cpp $(IMU_INC)
	$(CXX) $(CXX_FLAGS) -I$(IMU_DIR)/$(INC) $< -o $@ $(LIBS)



clean:
	rm -f $(OBJ)/*.o $(BIN_FILES)
