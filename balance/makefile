BASEDIR = ..
SRC = src
BIN = bin
OBJ = obj
INC = include
UTIL = $(BASEDIR)/util

CC = gcc
CXX = g++ -std=c++98
DEBUG = -g
CFLAGS = -Wall -Wextra -c $(DEBUG) -lpthread
CXX_FLAGS = -Wall -Wextra -c $(DEBUG) -lpthread
LFLAGS =  -Wall -Wextra $(DEBUG) -lpthread
LIBS = -lpigpio -lrt -l bcm2835 -l m


IMU_DIR = $(BASEDIR)/mpu6050_bcm2835
MOTOR_DIR = $(BASEDIR)/mdd10a

IMU_INC = $(IMU_DIR)/$(INC)/MPU6050.h $(IMU_DIR)/$(INC)/I2Cdev.h
MOTOR_INC = $(MOTOR_DIR)/$(INC)/PiMotor.h

IMU_OBJ = $(IMU_DIR)/$(OBJ)/MPU6050.o $(IMU_DIR)/$(OBJ)/I2Cdev.o
MOTOR_OBJ = $(MOTOR_DIR)/$(OBJ)/PiMotor.o

BIN_FILES = $(BIN)/balance_thread11 $(BIN)/balance_pthread $(BIN)/balance_timing $(BIN)/stop
OBJ_FILES = $(IMU_OBJ) $(MOTOR_OBJ)
all: $(BIN_FILES)



# balance bin
$(BIN)/balance_thread11: $(OBJ)/balance_thread11.o $(OBJ_FILES)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/balance_pthread: $(OBJ)/balance_pthread.o $(OBJ_FILES)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/balance_timing: $(OBJ)/balance_timing.o $(OBJ_FILES)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/stop: $(OBJ)/stop.o $(OBJ_FILES)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

# balance obj
$(OBJ)/%.o: $(TEST)/%.cpp
	$(CXX) $(CXX_FLAGS) -I$(INC) -I$(UTIL) -I$(IMU_DIR)/$(INC) -I$(MOTOR_DIR)/$(INC) $< -o $@

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXX_FLAGS) -I$(INC) -I$(UTIL) -I$(IMU_DIR)/$(INC) -I$(MOTOR_DIR)/$(INC) $< -o $@


# special case that needs c++11
$(OBJ)/balance_thread11.o: $(SRC)/balance_thread11.cpp
	g++ -std=c++11 $(CXX_FLAGS) -I$(INC) -I$(IMU_DIR)/$(INC) -I$(MOTOR_DIR)/$(INC) $< -o $@



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
