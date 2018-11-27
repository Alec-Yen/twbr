BIN = bin/
OBJ = obj/

all: motor 

clean:
	rm -f *.o $(BIN)motor

motor: motor.cpp
	g++ -c PiMotor.cpp 
	g++ -c motor.cpp 
	g++ -o $(BIN)motor motor.o PiMotor.o -lpigpio -lrt -lpthread
