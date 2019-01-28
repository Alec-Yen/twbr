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


HEADERS = $(INC)/MPU6050.h $(INC)/I2Cdev.h $(INC)/PiMotor.h $(INC)/twbr.h $(INC)/timing.h $(INC)/encoder.h

BALANCE_TARGETS = $(BIN)/balance_thread11 $(BIN)/balance_pthread $(BIN)/balance_timing $(BIN)/balance_final $(BIN)/balance_calibration
TEST_TARGETS = $(BIN)/stop $(BIN)/motor-test $(BIN)/imu-test $(BIN)/encoder-test $(BIN)/roam

TARGETS = $(BALANCE_TARGETS) $(TEST_TARGETS)

OBJECTS = $(OBJ)/MPU6050.o $(OBJ)/I2Cdev.o $(OBJ)/PiMotor.o $(INC)/twbr.o


all: $(TARGETS)



# balance targets
$(BIN)/balance_thread11: $(OBJ)/balance_thread11.o $(OBJECTS)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/balance_pthread: $(OBJ)/balance_pthread.o $(OBJECTS)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/balance_timing: $(OBJ)/balance_timing.o $(OBJECTS)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/balance_final: $(OBJ)/balance_final.o $(OBJECTS)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/balance_calibration: $(OBJ)/balance_calibration.o $(OBJECTS)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS
	)
$(BIN)/roam: $(OBJ)/roam.o $(OBJECTS)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

# test targets

$(BIN)/stop: $(OBJ)/stop.o $(OBJECTS)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/motor-test: $(OBJ)/motor-test.o $(OBJECTS)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/imu-test: $(OBJ)/imu-test.o $(OBJECTS)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)

$(BIN)/encoder-test: $(OBJ)/encoder-test.o $(OBJECTS)
	$(CXX) $(LFLAGS) $^ -o $@ $(LIBS)



# objects
$(OBJ)/%.o: $(TEST)/%.cpp
	$(CXX) $(CXX_FLAGS) -I$(INC) $< -o $@

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXX_FLAGS) -I$(INC) $< -o $@

$(OBJ)/%.o: $(INC)/%.cpp $(INC)/%.h
	$(CXX) $(CXX_FLAGS) -I$(INC) $< -o $@


# special case that needs c++11
$(OBJ)/balance_thread11.o: $(TEST)/balance_thread11.cpp
	g++ -std=c++11 $(CXX_FLAGS) -I$(INC) $< -o $@


clean:
	rm -f $(OBJ)/*.o $(TARGETS)
