BIN = bin/
OBJ = obj/
FLAGS = -std=c++11 -Wall

all: $(BIN)balance $(BIN)stop

clean:
	rm -f *.o $(BIN)balance $(BIN)stop

$(BIN)balance: balance.cpp
	g++ $(FLAGS) -o $(BIN)balance balance.cpp -I ../motors_pigpio ../motors_pigpio/PiMotor.cpp \
					-I  ../imu/cpp/Arduino_MPU6050 ../imu/cpp/Arduino_MPU6050/MPU6050.cpp \
	      	-I ../imu/cpp/RaspberryPi_bcm2835/I2Cdev ../imu/cpp/RaspberryPi_bcm2835/I2Cdev/I2Cdev.cpp \
					-lpigpio -lrt -lpthread -l bcm2835 -l m

$(BIN)stop: stop.cpp
	g++ $(FLAGS) -o $(BIN)stop stop.cpp -I ../motors_pigpio ../motors_pigpio/PiMotor.cpp \
					-lpigpio -lrt -lpthread
