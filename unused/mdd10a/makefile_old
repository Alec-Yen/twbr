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
LIBS = -lpigpio -lrt

BASEDIR = ..

BIN_FILES = $(BIN)/motor
	

all : $(BIN_FILES)


$(BIN)/motor: $(OBJ)/motor.o $(OBJ)/PiMotor.o
	$(CXX) $(LFLAGS) $^ $(LIBS) -o $@ 

$(OBJ)/motor.o: $(TEST)/motor.cpp $(SRC)/PiMotor.cpp $(INC)/PiMotor.h
	$(CXX) $(CXX_FLAGS) -I$(INC) $< -o $@

$(OBJ)/PiMotor.o: $(SRC)/PiMotor.cpp $(INC)/PiMotor.h
	$(CXX) $(CXX_FLAGS) -I$(INC) $< -o $@


clean:
	rm -f $(OBJ)/*.o $(BIN_FILES)

