SRC = src
BIN = bin
OBJ = obj
INC = include
TEST = test

CC = gcc
CXX = g++ -std=c++98
DEBUG = -g
CFLAGS = -O3 -Wall -c $(DEBUG) -lpthread
CXX_FLAGS = -O3 -Wall -c $(DEBUG) -lpthread
LFLAGS = -O3 -Wall $(DEBUG) -lpthread
LIBS = 

BASEDIR = ..

BIN_FILES = $(BIN)/rpi-mpu6050 $(BIN)/imu_readings
OBJ_FILES = $(OBJ)/MPU6050.o
INC_FILES = $(INC)/MPU6050.h


all : $(BIN_FILES)

$(OBJ)/%.o : $(TEST)/%.cpp $(INC_FILES)
	$(CXX) $(CXX_FLAGS) -I$(INC) -o $@ $< $(LIBS)

$(OBJ)/%.o : $(SRC)/%.cpp $(INC_FILES)
	$(CXX) $(CXX_FLAGS) -I$(INC) -o $@ $< $(LIBS)

$(OBJ)/%.o : $(TEST)/%.c $(INC_FILES)
	$(CC) $(CFLAGS) -I$(INC) -o $@ $< $(LIBS)

$(OBJ)/%.o : $(SRC)/%.c $(INC_FILES)
	$(CC) $(CFLAGS) -I$(INC) -o $@ $< $(LIBS)



$(BIN)/rpi-mpu6050: $(OBJ)/rpi-mpu6050.o $(OBJ_FILES)
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

$(BIN)/imu_readings: $(OBJ)/imu_readings.o $(OBJ_FILES)
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)



clean:
	rm -f $(OBJ)/*.o $(BIN_FILES)

