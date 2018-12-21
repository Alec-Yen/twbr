SRC = src
BIN = bin
OBJ = obj
INC = inc

CC = gcc
CXX = g++
DEBUG = -g
CFLAGS = -O2 -Wall -c $(DEBUG)
CXX_FLAGS = -O2 -Wall -c $(DEBUG)
LFLAGS = -O2 -Wall $(DEBUG)

ROBOCLAW = $(OBJ)/roboclaw.o
EXAMPLES = $(BIN)/roboclaw-test
	
all : $(ROBOCLAW) $(EXAMPLES)
	
examples: $(EXAMPLES)
			
$(BIN)/roboclaw-test : $(OBJ)/roboclaw.o $(OBJ)/roboclaw-test.o
	$(CC) $(LFLAGS) $(OBJ)/roboclaw.o $(OBJ)/roboclaw-test.o -o $(BIN)/roboclaw-test
		
$(OBJ)/roboclaw-test.o : $(INC)/roboclaw.h $(SRC)/roboclaw-test.c
	$(CC) $(CFLAGS) -I$(INC) $(SRC)/roboclaw-test.c -o $(OBJ)/roboclaw-test.o
		
$(OBJ)/roboclaw.o : $(INC)/roboclaw.h $(SRC)/roboclaw.c
	$(CC) $(CFLAGS) -I$(INC) $(SRC)/roboclaw.c -o $(OBJ)/roboclaw.o

clean:
	\rm -f *.o examples/*.o $(ROBOCLAW) $(EXAMPLES)
