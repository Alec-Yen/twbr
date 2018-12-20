BIN = bin/
OBJ = obj/
INCLUDE = -I./lcd_i2c/
LIBS = ./lcd_i2c/lcd_i2c.a
FLAGS = -std=c++11 -Wall

all: $(BIN)clock

clean:
	rm -f *.o $(BIN)clock

$(BIN)clock: clock.cpp
	g++ -o $(BIN)clock clock.cpp $(INCLUDE) $(LIBS)
