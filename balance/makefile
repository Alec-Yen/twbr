BIN = bin/
OBJ = obj/

all: $(BIN)balance $(BIN)stop $(BIN)ay

clean:
	rm -f *.o $(BIN)balance $(BIN)stop

$(BIN)balance: balance.cpp
	g++ -o $(BIN)balance balance.cpp -I ../motordriver ../motordriver/PiMotor.cpp \
					-I  ../imu/cpp/Arduino_MPU6050 ../imu/cpp/Arduino_MPU6050/MPU6050.cpp \
	      	-I ../imu/cpp/RaspberryPi_bcm2835/I2Cdev ../imu/cpp/RaspberryPi_bcm2835/I2Cdev/I2Cdev.cpp \
					-lpigpio -lrt -lpthread -l bcm2835 -l m

$(BIN)ay: balance_ay.cpp
	g++ -o $(BIN)ay balance_ay.cpp -I ../motordriver ../motordriver/PiMotor.cpp \
					-I  ../imu/cpp/Arduino_MPU6050 ../imu/cpp/Arduino_MPU6050/MPU6050.cpp \
	      	-I ../imu/cpp/RaspberryPi_bcm2835/I2Cdev ../imu/cpp/RaspberryPi_bcm2835/I2Cdev/I2Cdev.cpp \
					-lpigpio -lrt -lpthread -l bcm2835 -l m

$(BIN)stop: stop.cpp
	g++ -o $(BIN)stop stop.cpp -I ../motordriver ../motordriver/PiMotor.cpp \
					-lpigpio -lrt -lpthread
