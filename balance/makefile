BASEDIR = ..
SRC = src
BIN = bin
OBJ = obj
INC = include
TEST = test

CC = gcc
CXX = g++ -std=c++98
DEBUG = -g
CFLAGS = -Wall -Wextra -c $(DEBUG) -lpthread
CXX_FLAGS = -Wall -Wextra -c $(DEBUG) -lpthread
LFLAGS =  -Wall -Wextra $(DEBUG) -lpthread
LIBS = -lpigpio -lrt -lbcm2835 -lwiringPi


HEADERS = $(INC)/MPU6050.h $(INC)/I2Cdev.h $(INC)/PiMotor.h $(INC)/twbr.h $(INC)/timing.h

TARGETS = $(BIN)/balance_thread11 $(BIN)/balance_pthread $(BIN)/balance_timing $(BIN)/stop $(BIN)/rotaryencoder $(BIN)/motor-test $(BIN)/imu_readings $(BIN)/encoder-test

OBJECTS = $(OBJ)/MPU6050.o $(OBJ)/I2Cdev.o $(OBJ)/PiMotor.o $(INC)/twbr.o


all: $(TARGETS)



# bins
$(BIN)/balance_thread11: $(OBJ)/balance_thread11.o $(OBJECTS)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/balance_pthread: $(OBJ)/balance_pthread.o $(OBJECTS)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/balance_timing: $(OBJ)/balance_timing.o $(OBJECTS)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/stop: $(OBJ)/stop.o $(OBJECTS)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/rotaryencoder: $(OBJ)/rotaryencoder.o $(OBJECTS)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/motor-test: $(OBJ)/motor-test.o $(OBJECTS)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/imu_readings: $(OBJ)/imu_readings.o $(OBJECTS)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/encoder-test: $(OBJ)/encoder-test.o $(OBJECTS)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)



# objects
$(OBJ)/%.o: $(TEST)/%.cpp
	$(CXX) $(CXX_FLAGS) -I$(INC) $< -o $@

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXX_FLAGS) -I$(INC) $< -o $@

$(OBJ)/%.o: $(INC)/%.cpp
	$(CXX) $(CXX_FLAGS) -I$(INC) $< -o $@


# special case that needs c++11
$(OBJ)/balance_thread11.o: $(TEST)/balance_thread11.cpp
	g++ -std=c++11 $(CXX_FLAGS) -I$(INC) $< -o $@


clean:
	rm -f $(OBJ)/*.o $(TARGETS)
