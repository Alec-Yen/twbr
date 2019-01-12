BIN = bin
OBJ = obj
INC = include
SRC = src
LIBS = $(INC)/LiquidCrystal_I2C.a

CXX = g++ -std=c++11
CXX_FLAGS = -g -Wall

all: $(BIN)/clock

clean:
	rm -f *.o $(BIN)/clock

$(BIN)/clock: $(SRC)/clock.cpp
	$(CXX) $(CXX_FLAGS) -o $(BIN)/clock $(SRC)/clock.cpp -I$(INC) $(LIBS)
