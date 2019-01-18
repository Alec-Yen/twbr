SRC = src
BIN = bin
OBJ = obj
INC = include
TEST = test

CC = gcc
CXX = g++ -std=c++98
DEBUG = -g
CFLAGS = -O3 -Wall -c $(DEBUG) -pthread -lwiringPi -lpigpio
CXX_FLAGS = -O3 -Wall -c $(DEBUG) -pthread -lwiringPi -lpigpio
LFLAGS = -O3 -Wall $(DEBUG) -pthread -lwiringPi -lpigpio

BIN_FILES = $(BIN)/motor $(BIN)/rotaryencoder
INC_FILES = $(INC)/PiMotor.h
	

all : $(BIN_FILES)

# object files

$(OBJ)/%.o: $(TEST)/%.cpp $(INC_FILES) 
	$(CXX) $(CXX_FLAGS) -I$(INC) $< -o $@

$(OBJ)/%.o: $(SRC)/%.cpp $(INC_FILES)
	$(CXX) $(CXX_FLAGS) -I$(INC) $< -o $@

$(OBJ)/%.o: $(TEST)/%.c $(INC_FILES)
	$(CC) $(CFLAGS) -I$(INC) $< -o $@

$(OBJ)/%.o: $(SRC)/%.c $(INC_FILES)
	$(CC) $(CFLAGS) -I$(INC) $< -o $@


# binary files

$(BIN)/motor : $(OBJ)/motor.o $(OBJ)/PiMotor.o
	$(CXX) $(LFLAGS) $^ -o $@

$(BIN)/rotaryencoder : $(OBJ)/rotaryencoder.o $(OBJ)/PiMotor.o
	$(CC) $(LFLAGS) $^ -o $@



clean:
	rm -f $(OBJ)/*.o $(BIN_FILES)
