BIN = bin
OBJ = obj
LCD = -I../lcd/lcd_i2c/ ../lcd/lcd_i2c/lcd_i2c.a
FLAGS = -std=c++11 -Wall

all: $(BIN)/ultrasonic  $(BIN)/lcd_print

clean:
	rm -f *.o $(BIN)/ultrasonic

$(BIN)/ultrasonic: ultrasonic.cpp
	g++ $(FLAGS) -o $(BIN)/ultrasonic ultrasonic.cpp -lpigpio -lrt -lpthread

$(BIN)/lcd_print: lcd_print.cpp
	g++ $(FLAGS) -o $(BIN)/lcd_print lcd_print.cpp $(LCD) -lpigpio -lrt -lpthread
