BASEDIR = ..
SRC = src
BIN = bin
OBJ = obj
INC = include
TEST = test
UTIL = $(BASEDIR)/util

CC = gcc
CXX = g++ -std=c++98
DEBUG = -g
CFLAGS = -Wall -Wextra -c $(DEBUG) -lpthread
CXX_FLAGS = -Wall -Wextra -c $(DEBUG) -lpthread
LFLAGS =  -Wall -Wextra $(DEBUG) -lpthread
LIBS = -lpigpio -lrt -l bcm2835 -l m


IMU_DIR = $(BASEDIR)/mpu6050_bcm2835
MOTOR_DIR = $(BASEDIR)/roboclaw

IMU_INC = $(IMU_DIR)/$(INC)/MPU6050.h $(IMU_DIR)/$(INC)/I2Cdev.h
MOTOR_INC = $(MOTOR_DIR)/$(INC)/roboclaw.h $(MOTOR_DIR)/$(INC)/rc_twbr.hpp

IMU_OBJ = $(IMU_DIR)/$(OBJ)/MPU6050.o $(IMU_DIR)/$(OBJ)/I2Cdev.o
MOTOR_OBJ = $(MOTOR_DIR)/$(OBJ)/roboclaw.o $(MOTOR_DIR)/$(OBJ)/rc_twbr.o

INC_FILES = $(INC)/twbr.h $(INC)/pid.h 
BIN_FILES = $(BIN)/timing $(BIN)/balance_timing $(BIN)/balance_timing_calibrate $(BIN)/balance $(BIN)/balance_pid $(BIN)/roam $(BIN)/balance_y
OBJ_FILES = $(IMU_OBJ) $(MOTOR_OBJ) $(OBJ)/twbr.o $(OBJ)/pid.o
all: $(BIN_FILES)



# balance bin
$(BIN)/timing: $(OBJ)/timing.o $(OBJ_FILES)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/balance_timing_calibrate: $(OBJ)/balance_timing_calibrate.o $(OBJ_FILES)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/balance_timing: $(OBJ)/balance_timing.o $(OBJ_FILES)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/balance_y: $(OBJ)/balance_y.o $(OBJ_FILES)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/balance: $(OBJ)/balance.o $(OBJ_FILES)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/balance_pid: $(OBJ)/balance_pid.o $(OBJ_FILES)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/roam: $(OBJ)/roam.o $(OBJ_FILES)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

# balance obj
$(OBJ)/%.o: $(TEST)/%.cpp $(INC_FILES)
	$(CXX) $(CXX_FLAGS) -I$(INC) -I$(UTIL) -I$(IMU_DIR)/$(INC) -I$(MOTOR_DIR)/$(INC) $< -o $@

$(OBJ)/%.o: $(SRC)/%.cpp $(INC_FILES)
	$(CXX) $(CXX_FLAGS) -I$(INC) -I$(UTIL) -I$(IMU_DIR)/$(INC) -I$(MOTOR_DIR)/$(INC) $< -o $@



# motor obj
$(MOTOR_DIR)/$(OBJ)/%.o: $(MOTOR_DIR)/$(TEST)/%.cpp $(MOTOR_INC) 
	$(CXX) $(CXX_FLAGS) -I$(MOTOR_DIR)/$(INC) $< -o $@

$(MOTOR_DIR)/$(OBJ)/%.o: $(MOTOR_DIR)/$(SRC)/%.cpp $(MOTOR_INC)
	$(CXX) $(CXX_FLAGS) -I$(MOTOR_DIR)/$(INC) $< -o $@

$(MOTOR_DIR)/$(OBJ)/%.o: $(MOTOR_DIR)/$(TEST)/%.c $(MOTOR_INC)
	$(CC) $(CFLAGS) -I$(MOTOR_DIR)/$(INC) $< -o $@

$(MOTOR_DIR)/$(OBJ)/%.o: $(MOTOR_DIR)/$(SRC)/%.c $(MOTOR_INC)
	$(CC) $(CFLAGS) -I$(MOTOR_DIR)/$(INC) $< -o $@

# imu obj
$(IMU_DIR)/$(OBJ)/%.o: $(IMU_DIR)/$(SRC)/%.cpp $(IMU_INC)
	$(CXX) $(CXX_FLAGS) -I$(IMU_DIR)/$(INC) $< -o $@



clean:
	rm -f $(OBJ)/*.o $(BIN_FILES)
