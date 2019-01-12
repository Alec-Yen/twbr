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

BASEDIR = .. # FIXME

BIN_FILES = $(BIN)/test_file # FIXME
OBJ_FILES = $(OBJ)/src_file.o # FIXME
INC_FILES = $(INC)/src_file.h # FIXME


all : $(BIN_FILES)


# object files (C++ and C)

$(OBJ)/%.o : $(TEST)/%.cpp $(INC_FILES)
	$(CXX) $(CXX_FLAGS) -I$(INC) -o $@ $< $(LIBS)

$(OBJ)/%.o : $(SRC)/%.cpp $(INC_FILES)
	$(CXX) $(CXX_FLAGS) -I$(INC) -o $@ $< $(LIBS)

$(OBJ)/%.o : $(TEST)/%.c $(INC_FILES)
	$(CC) $(CFLAGS) -I$(INC) -o $@ $< $(LIBS)

$(OBJ)/%.o : $(SRC)/%.c $(INC_FILES)
	$(CC) $(CFLAGS) -I$(INC) -o $@ $< $(LIBS)


# bin files FIXME

$(BIN)/test_file: $(OBJ)/test_file.o $(OBJ_FILES)
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)


clean:
	rm -f $(OBJ)/*.o $(BIN_FILES)




