/************************************************************
MPU9250_FIFO_Basic

This example sketch demonstrates how to use the MPU-9250's
512 byte first-in, first-out (FIFO) buffer. The FIFO can be
set to store either accelerometer and/or gyroscope (not the
magnetometer, though :( ).

Timing of the FIFO buffer handling is quite high, because
the buffer takes time to be filled and in this example 256
bytes are enough to contain more than one sample.
Results, from the point of view of the time spent to access
the FIFO, is absolutely not satisfactory.

Results:
- 370 us: fifoAvailable() >= 256
- 370 us: while (imu.fifoAvailable() > 0)
- 683 us: imu.updateFifo()

Acknowledges:
Original code by Jim Lindblom @ SparkFun Electronics
original creation date: November 23, 2016
https://github.com/sparkfun/SparkFun_MPU9250_DMP_Arduino_Library
*************************************************************/
#include "MPU9250.h"

#define SerialPort SerialUSB

MPU9250_DMP imu;

void setup()
{
	SerialPort.begin(460800);

	// Call imu.begin() to verify communication with and
	// initialize the MPU-9250 to it's default values.
	// Most functions return an error code - INV_SUCCESS (0)
	// indicates the IMU was present and successfully set up
	if (imu.begin() != INV_SUCCESS)
	{
		while (1)
		{
			SerialPort.println("Unable to communicate with MPU-9250");
			SerialPort.println("Check connections, and try again.");
			SerialPort.println();
			delay(5000);
		}
	}

	// Use setGyroFSR() and setAccelFSR() to configure the
	// gyroscope and accelerometer full scale ranges.
	// Gyro options are +/- 250, 500, 1000, or 2000 dps
	imu.setGyroFSR(250); // Set gyro to 250 dps
						 // Accel options are +/- 2, 4, 8, or 16 g
	imu.setAccelFSR(2); // Set accel to +/-2g
						// Note: the MPU-9250's magnetometer FSR is set at 
						// +/- 4912 uT (micro-tesla's)

						// setLPF() can be used to set the digital low-pass filter
						// of the accelerometer and gyroscope.
						// Can be any of the following: 188, 98, 42, 20, 10, 5
						// (values are in Hz).
	imu.setLPF(42); // Set LPF corner frequency to 42Hz

					// The sample rate of the accel/gyro can be set using
					// setSampleRate. Acceptable values range from 4Hz to 1kHz
	imu.setSampleRate(100); // Set sample rate to 100Hz

	//						// Likewise, the compass (magnetometer) sample rate can be
	//						// set using the setCompassSampleRate() function.
	//						// This value can range between: 1-100Hz
	//imu.setCompassSampleRate(100); // Set mag rate to 100Hz

	//// Use setSensors to turn on or off MPU-9250 sensors.
	//// Any of the following defines can be combined:
	//// INV_XYZ_GYRO, INV_XYZ_ACCEL, INV_XYZ_COMPASS,
	//// INV_X_GYRO, INV_Y_GYRO, or INV_Z_GYRO
	//// Enable all sensors:
	//imu.setSensors(INV_XYZ_COMPASS);

	// Use configureFifo to set which sensors should be stored
	// in the buffer.  
	// Parameter to this function can be: INV_XYZ_GYRO, 
	// INV_XYZ_ACCEL, INV_X_GYRO, INV_Y_GYRO, or INV_Z_GYRO
	imu.configureFifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
}

unsigned int t[5];
void loop()
{
	// fifoAvailable returns the number of bytes in the FIFO
	// The FIFO is 512 bytes max. We'll read when it reaches
	// half of that.
	t[0] = micros();
	if (imu.fifoAvailable() >= 256)
	{
		t[1] = micros();
		// Then read while there is data in the FIFO
		while (imu.fifoAvailable() > 0)
		{
			t[2] = micros();
			// Call updateFifo to update ax, ay, az, gx, gy, and/or gz
			if (imu.updateFifo() == INV_SUCCESS)
			{
				t[3] = micros();
				printTiming(3);
				printIMUData();
			}
			else
			{
				t[3] = micros();
				printTiming(3);
			}
		}
		t[2] = micros();
		printTiming(2);
	}
	else
	{
		t[1] = micros();
		printTiming(1);
	}
}

void printTiming(int mpuRead)
{
	for (int i=1; i<=mpuRead; i++)
	{
		if (i == 1)
			SerialPort.printf("0-1=%d",
				t[i] - t[i - 1]);
		else
			SerialPort.printf(";%d-%d=%d",
				i, i - 1, t[i] - t[i - 1]);
	}
	SerialPort.println();
}

char output[256];
unsigned int printCount = 0;
void printIMUData(void)
{
	if (printCount++ % 10 != 0)
		return;

	// After calling update() the internal imu data is 
	// updated. 

	// Use the getData function to convert and retrieve 
	// sensor readings in their respective physical units.
	float a[3], g[3], m[3], T;
	imu.getData(a, g, m, T);

	SerialPort.printf("MPU:a=[% 03.2f,% 03.2f,% 03.2f]g,",
		a[X_AXIS], a[Y_AXIS], a[Z_AXIS]);
	SerialPort.printf("o=[% 03.2f,% 03.2f,% 03.2f]dps,",
		g[X_AXIS], g[Y_AXIS], g[Z_AXIS]);
	SerialPort.printf("m=[% 03.2f,% 03.2f,% 03.2f]uT,",
		m[X_AXIS], m[Y_AXIS], m[Z_AXIS]);
	SerialPort.printf("T=%2.1f,t=%lums\r\n",
		T, imu.time);
}