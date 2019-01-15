#ifndef MPU6050_H
#define MPU6050_H

#include <stdio.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <string>

#define MPU_ACCEL_XOUT1 0x3b
#define MPU_ACCEL_XOUT2 0x3c
#define MPU_ACCEL_YOUT1 0x3d
#define MPU_ACCEL_YOUT2 0x3e
#define MPU_ACCEL_ZOUT1 0x3f
#define MPU_ACCEL_ZOUT2 0x40

#define MPU_GYRO_XOUT1 0x43
#define MPU_GYRO_XOUT2 0x44
#define MPU_GYRO_YOUT1 0x45
#define MPU_GYRO_YOUT2 0x46
#define MPU_GYRO_ZOUT1 0x47
#define MPU_GYRO_ZOUT2 0x48

#define MPU_TEMP1 0x41
#define MPU_TEMP2 0x42

#define MPU_POWER1 0x6b
#define MPU_POWER2 0x6c

class MPU6050 {
	public:
		void initialize();
		int16_t getAccelerationX();
		int16_t getAccelerationY();
		int16_t getAccelerationZ();
		int16_t getRotationX();
		int16_t getRotationY();
		int16_t getRotationZ();
		void getAcceleration(int16_t& x, int16_t& y, int16_t& z);
		void getRotation(int16_t& x, int16_t& y, int16_t& z);
		void getMotion6(int16_t& ax, int16_t& ay, int16_t& az, int16_t& gx, int16_t& gy, int16_t& gz);
		float getTemp();

	protected:
		const char* fileName;
		int address;
		int8_t power;
		int fd;
};

#endif // MPU6050_H
