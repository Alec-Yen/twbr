BIN = bin/
OBJ = obj/

all: motor 

clean:
	rm -f *.o $(BIN)motor

motor: motor.cpp
	g++ -o $(BIN)motor -I. PiMotor.cpp motor.cpp -lpigpio -lrt -lpthread
