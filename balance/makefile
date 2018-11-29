BIN = bin/
OBJ = obj/

all: main 

clean:
	rm -f *.o $(BIN)main

main: main.cpp
	g++ -c PiMotor.cpp 
	g++ -c main.cpp 
	g++ -o $(BIN)main main.o PiMotor.o -lpigpio -lrt -lpthread
