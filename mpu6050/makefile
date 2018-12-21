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
LIBS = -l bcm2835 -l m

BASEDIR = ..

BIN_FILES = $(BIN)/MPU6050_example_1
OBJ_FILES = $(OBJ)/MPU6050.o $(OBJ)/I2Cdev.o $(OBJ)/MPU6050_example_1.o
	

all : $(BIN_FILES)


$(BIN)/MPU6050_example_1: $(OBJ_FILES)
	$(CC) $(LFLAGS) -o $(BIN)/MPU6050_example_1 $(OBJ_FILES) $(LIBS)

$(OBJ)/MPU6050_example_1.o: $(TEST)/MPU6050_example_1.cpp
	$(CC) $(CXX_FLAGS) -o $(OBJ)/MPU6050_example_1.o -I$(INC) $(TEST)/MPU6050_example_1.cpp $(LIBS)

$(OBJ)/MPU6050.o: $(SRC)/MPU6050.cpp $(INC)/MPU6050.h
	$(CC) $(CXX_FLAGS) -o $(OBJ)/MPU6050.o -I$(INC) $(SRC)/MPU6050.cpp $(LIBS)

$(OBJ)/I2Cdev.o: $(SRC)/I2Cdev.cpp $(INC)/I2Cdev.h
	$(CC) $(CXX_FLAGS) -o $(OBJ)/I2Cdev.o -I$(INC) $(SRC)/I2Cdev.cpp $(LIBS)

clean:
	rm -f $(OBJ)/*.o $(BIN_FILES)




