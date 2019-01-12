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

BIN_FILES = $(BIN)/encoders $(BIN)/encoders_thread $(BIN)/encoders_thread_safe $(BIN)/roboclaw-test
INC_FILES = $(INC)/roboclaw.h $(INC)/rc_twbr.hpp
	

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

$(BIN)/encoders_thread_safe : $(OBJ)/rc_twbr.o $(OBJ)/roboclaw.o $(OBJ)/encoders_thread_safe.o
	$(CXX) $(LFLAGS) $^ -o $@

$(BIN)/encoders_thread : $(OBJ)/rc_twbr.o $(OBJ)/roboclaw.o $(OBJ)/encoders_thread.o
	$(CXX) $(LFLAGS) $^ -o $@

$(BIN)/encoders : $(OBJ)/rc_twbr.o $(OBJ)/roboclaw.o $(OBJ)/encoders.o
	$(CXX) $(LFLAGS) $^ -o $@

$(BIN)/roboclaw-test : $(OBJ)/roboclaw.o $(OBJ)/roboclaw-test.o
	$(CC) $(LFLAGS) $^ -o $@


clean:
	rm -f $(OBJ)/*.o $(BIN_FILES)
