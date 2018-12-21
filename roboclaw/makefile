SRC = src
BIN = bin
OBJ = obj
INC = include

CC = gcc
CXX = g++ -std=c++98
DEBUG = -g
CFLAGS = -O3 -Wall -c $(DEBUG)
CXX_FLAGS = -O3 -Wall -c $(DEBUG)
LFLAGS = -O3 -Wall $(DEBUG)

TWBR = $(BIN)/encoders
EXAMPLES = $(BIN)/roboclaw-test
	

all : $(TWBR) $(EXAMPLES)
examples: $(EXAMPLES)

$(BIN)/encoders : $(OBJ)/roboclaw.o $(OBJ)/encoders.o
	$(CXX) $(LFLAGS) $(OBJ)/roboclaw.o $(OBJ)/encoders.o -o $(BIN)/encoders

$(OBJ)/encoders.o: $(SRC)/encoders.cpp
	$(CXX) $(CXX_FLAGS) -I$(INC) $(SRC)/encoders.cpp -o $(OBJ)/encoders.o

$(OBJ)/roboclaw.o : $(INC)/roboclaw.h $(SRC)/roboclaw.c
	$(CC) $(CFLAGS) -I$(INC) $(SRC)/roboclaw.c -o $(OBJ)/roboclaw.o




$(BIN)/roboclaw-test : $(OBJ)/roboclaw.o $(OBJ)/roboclaw-test.o
	$(CC) $(LFLAGS) $(OBJ)/roboclaw.o $(OBJ)/roboclaw-test.o -o $(BIN)/roboclaw-test
		
$(OBJ)/roboclaw-test.o : $(INC)/roboclaw.h $(SRC)/roboclaw-test.c
	$(CC) $(CFLAGS) -I$(INC) $(SRC)/roboclaw-test.c -o $(OBJ)/roboclaw-test.o
		




clean:
	\rm -f $(OBJ)/*.o $(ROBOCLAW) $(EXAMPLES) $(TWBR)
