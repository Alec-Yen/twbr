#include "MPU6050.h"

void MPU6050::initialize()
{
	std::string file = "/dev/i2c-1";
	fileName = file.c_str();
	address = 0x68;

	if ((fd = open(fileName, O_RDWR)) < 0) {
		printf("Failed to open i2c port\n");
		exit(1);
	}

	if (ioctl(fd, I2C_SLAVE, address) < 0) {
		printf("Unable to get bus access to talk to slave\n");
		exit(1);
	}

	power = i2c_smbus_read_byte_data(fd, MPU_POWER1);
	i2c_smbus_write_byte_data(fd, MPU_POWER1, ~(1 << 6) & power);
}

int16_t MPU6050::getAccelerationX()
{
	int16_t xaccel = i2c_smbus_read_byte_data(fd, MPU_ACCEL_XOUT1) << 8 |
		i2c_smbus_read_byte_data(fd, MPU_ACCEL_XOUT2);
	return xaccel;
}

int16_t MPU6050::getAccelerationY()
{
	int16_t yaccel = i2c_smbus_read_byte_data(fd, MPU_ACCEL_YOUT1) << 8 |
		i2c_smbus_read_byte_data(fd, MPU_ACCEL_YOUT2);
	return yaccel;
}

int16_t MPU6050::getAccelerationZ()
{
	int16_t zaccel = i2c_smbus_read_byte_data(fd, MPU_ACCEL_ZOUT1) << 8 |
		i2c_smbus_read_byte_data(fd, MPU_ACCEL_ZOUT2);
	return zaccel;
}

int16_t MPU6050::getRotationX()
{

	int16_t xgyro = i2c_smbus_read_byte_data(fd, MPU_GYRO_XOUT1) << 8 |
		i2c_smbus_read_byte_data(fd, MPU_GYRO_XOUT2);
	return xgyro;
}

int16_t MPU6050::getRotationY()
{
	int16_t ygyro = i2c_smbus_read_byte_data(fd, MPU_GYRO_YOUT1) << 8 |
		i2c_smbus_read_byte_data(fd, MPU_GYRO_YOUT2);
	return ygyro;
}

int16_t MPU6050::getRotationZ()
{
	int16_t zgyro = i2c_smbus_read_byte_data(fd, MPU_GYRO_ZOUT1) << 8 |
		i2c_smbus_read_byte_data(fd, MPU_GYRO_ZOUT2);
	return zgyro;
}

void MPU6050::getAcceleration(int16_t& xaccel, int16_t& yaccel, int16_t& zaccel)
{
	xaccel = i2c_smbus_read_byte_data(fd, MPU_ACCEL_XOUT1) << 8 |
		i2c_smbus_read_byte_data(fd, MPU_ACCEL_XOUT2);
	yaccel = i2c_smbus_read_byte_data(fd, MPU_ACCEL_YOUT1) << 8 |
		i2c_smbus_read_byte_data(fd, MPU_ACCEL_YOUT2);
	zaccel = i2c_smbus_read_byte_data(fd, MPU_ACCEL_ZOUT1) << 8 |
		i2c_smbus_read_byte_data(fd, MPU_ACCEL_ZOUT2);
}

void MPU6050::getRotation(int16_t& xgyro, int16_t& ygyro, int16_t& zgyro)
{
	xgyro = i2c_smbus_read_byte_data(fd, MPU_GYRO_XOUT1) << 8 |
		i2c_smbus_read_byte_data(fd, MPU_GYRO_XOUT2);
	ygyro = i2c_smbus_read_byte_data(fd, MPU_GYRO_YOUT1) << 8 |
		i2c_smbus_read_byte_data(fd, MPU_GYRO_YOUT2);
	zgyro = i2c_smbus_read_byte_data(fd, MPU_GYRO_ZOUT1) << 8 |
		i2c_smbus_read_byte_data(fd, MPU_GYRO_ZOUT2);
}

void MPU6050::getMotion6(int16_t& xaccel, int16_t& yaccel, int16_t& zaccel, int16_t& xgyro, int16_t& ygyro, int16_t& zgyro)
{
	xaccel = i2c_smbus_read_byte_data(fd, MPU_ACCEL_XOUT1) << 8 |
		i2c_smbus_read_byte_data(fd, MPU_ACCEL_XOUT2);
	yaccel = i2c_smbus_read_byte_data(fd, MPU_ACCEL_YOUT1) << 8 |
		i2c_smbus_read_byte_data(fd, MPU_ACCEL_YOUT2);
	zaccel = i2c_smbus_read_byte_data(fd, MPU_ACCEL_ZOUT1) << 8 |
		i2c_smbus_read_byte_data(fd, MPU_ACCEL_ZOUT2);
	xgyro = i2c_smbus_read_byte_data(fd, MPU_GYRO_XOUT1) << 8 |
		i2c_smbus_read_byte_data(fd, MPU_GYRO_XOUT2);
	ygyro = i2c_smbus_read_byte_data(fd, MPU_GYRO_YOUT1) << 8 |
		i2c_smbus_read_byte_data(fd, MPU_GYRO_YOUT2);
	zgyro = i2c_smbus_read_byte_data(fd, MPU_GYRO_ZOUT1) << 8 |
		i2c_smbus_read_byte_data(fd, MPU_GYRO_ZOUT2);
}

float MPU6050::getTemp()
{
	int16_t temp = i2c_smbus_read_byte_data(fd, MPU_TEMP1) << 8 |
		i2c_smbus_read_byte_data(fd, MPU_TEMP2);
	return (float)temp/ 340.0f + 36.53;
}
