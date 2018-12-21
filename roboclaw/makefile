SRC = src
BIN = bin
OBJ = obj
INC = include

CC = gcc
CXX = g++ -std=c++98
DEBUG = -g
CFLAGS = -O3 -Wall -c $(DEBUG) -lpthread
CXX_FLAGS = -O3 -Wall -c $(DEBUG) -lpthread
LFLAGS = -O3 -Wall $(DEBUG) -lpthread

TWBR = $(BIN)/encoders $(BIN)/encoders_thread
EXAMPLES = $(BIN)/roboclaw-test
	

all : $(TWBR) $(EXAMPLES)
examples: $(EXAMPLES)

$(BIN)/encoders_thread : $(OBJ)/roboclaw.o $(OBJ)/encoders_thread.o
	$(CXX) $(LFLAGS) $(OBJ)/roboclaw.o $(OBJ)/encoders_thread.o -o $(BIN)/encoders_thread

$(OBJ)/encoders_thread.o: $(SRC)/encoders_thread.cpp
	$(CXX) $(CXX_FLAGS) -I$(INC) $(SRC)/encoders_thread.cpp -o $(OBJ)/encoders_thread.o

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
	rm -f $(OBJ)/*.o $(EXAMPLES) $(TWBR)
