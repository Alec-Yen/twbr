BIN = bin
OBJ = obj

all: $(BIN)/motor 

clean:
	rm -f *.o $(BIN)/motor

$(BIN)/motor: motor.cpp
	g++ -o $(BIN)/motor -I. PiMotor.cpp motor.cpp -lpigpio -lrt -lpthread
