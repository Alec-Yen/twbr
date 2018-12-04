BIN = bin/
OBJ = obj/

all: balance stop

clean:
	rm -f *.o $(BIN)balance $(BIN)stop

balance: balance.cpp
	g++ -o $(BIN)balance balance.cpp -I ../motordriver ../motordriver/PiMotor.cpp \
					-I  ../imu/cpp/Arduino_MPU6050 ../imu/cpp/Arduino_MPU6050/MPU6050.cpp \
	      	-I ../imu/cpp/RaspberryPi_bcm2835/I2Cdev ../imu/cpp/RaspberryPi_bcm2835/I2Cdev/I2Cdev.cpp \
					-lpigpio -lrt -lpthread -l bcm2835 -l m

stop: stop.cpp
	g++ -o $(BIN)stop stop.cpp -I ../motordriver ../motordriver/PiMotor.cpp \
					-lpigpio -lrt -lpthread
