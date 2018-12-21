SRC = src
BIN = bin
OBJ = obj
INC = $(BASEDIR)/lcd/include
STATIC_LIBS = $(INC)/LiquidCrystal_I2C.a
LIBS = -lpigpio -lrt -lpthread

BASEDIR = ..

CC = gcc
CXX = g++ -std=c++98
DEBUG = -g
CFLAGS = -O3 -Wall -c $(DEBUG)
CXX_FLAGS = -O3 -Wall -c $(DEBUG)
LFLAGS = -O3 -Wall $(DEBUG)



BIN_FILES = $(BIN)/ultrasonic  $(BIN)/lcd_print

all: $(BIN_FILES)


$(BIN)/ultrasonic: $(SRC)/ultrasonic.cpp
	$(CXX) $(LFLAGS) -o $(BIN)/ultrasonic $(SRC)/ultrasonic.cpp $(LIBS)

$(BIN)/lcd_print: $(SRC)/lcd_print.cpp
	$(CXX) $(LFLAGS) -o $(BIN)/lcd_print $(SRC)/lcd_print.cpp -I$(INC) $(STATIC_LIBS) $(LIBS)


clean:
	rm -f $(OBJ)/*.o $(BIN_FILES)




