BIN = bin
SRC = src
OBJ = obj
FLAGS = -std=c++11 -Wall
LIBS = -lpigpio -lrt -lpthread -l bcm2835 -l m

IMU =  -I  ../imu/cpp/Arduino_MPU6050 ../imu/cpp/Arduino_MPU6050/MPU6050.cpp -I ../imu/cpp/RaspberryPi_bcm2835/I2Cdev ../imu/cpp/RaspberryPi_bcm2835/I2Cdev/I2Cdev.cpp
MOTOR = -I ../motors_pigpio ../motors_pigpio/PiMotor.cpp

all: $(BIN)/balance
balance: $(BIN)/balance
stop: $(BIN)/stop

clean:
	rm -f *.o $(BIN)/balance $(BIN)/stop

$(BIN)/balance: $(SRC)/balance.cpp
	g++ $(FLAGS) -o $(BIN)/balance $(SRC)/balance.cpp $(MOTOR) $(IMU) $(LIBS)

$(BIN)/stop: $(SRC)/stop.cpp
	g++ $(FLAGS) -o $(BIN)/stop $(SRC)/stop.cpp $(MOTOR) $(LIBS)
