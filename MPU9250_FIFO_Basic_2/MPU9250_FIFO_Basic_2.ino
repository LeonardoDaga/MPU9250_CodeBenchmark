/************************************************************
MPU9250_FIFO_Basic_2

This example shows a better interaction with the fifo buffer.
FIFO buffer is mainly used to accumulate samples when the 
processor has no time to manage them.
Instead of asking every step the number of bytes available,
the code starts from the assumpion that no further bytes 
add to the buffer while parsing the FIFO.
At this time, I didn't find a way to retrieve also magnetometers, 
even at lower frequency or not using FIFO, when FIFO is used 
for acceleometers and gyros. 

Timing of the FIFO buffer handling is quite high, no gain
is achieved respect to the interrupt case. This is a limit
of the I2C bus that cannot transmit data in background.

Results:
- 370 us: fifoAvailable() >= 12
- 685 us: imu.updateFifo()
- 211 us: imu.updateTemperature();

Acknowledges:
Original code by Jim Lindblom @ SparkFun Electronics
original creation date: November 23, 2016
https://github.com/sparkfun/SparkFun_MPU9250_DMP_Arduino_Library
*************************************************************/
#include "MPU9250.h"

#define SerialPort SerialUSB
#define MSG_SIZE 12

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

	// Magnetometer and temperature sensor cannot be retrieved 
	// using the FIFO
	imu.

	// Use configureFifo to set which sensors should be stored
	// in the buffer.  
	// Parameter to this function can be: INV_XYZ_GYRO, 
	// INV_XYZ_ACCEL, INV_X_GYRO, INV_Y_GYRO, or INV_Z_GYRO
	imu.configureFifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
}

unsigned int t[5];
int bytes = 0;

void loop()
{
	// fifoAvailable returns the number of bytes in the FIFO
	// The FIFO is 512 bytes max. We'll read when there is 
	// at least one message
	t[0] = micros();
	if ((bytes = imu.fifoAvailable()) >= 12)
	{
		t[1] = micros();

		// Loop until there are more than twelve bytes in the FIFO
		while (bytes >= 12)
		{
			// Call updateFifo to update ax, ay, az, gx, gy, and/or gz
			if (imu.updateFifo() == INV_SUCCESS)
			{
				t[2] = micros();

				// Here we retrieve the temperature, at least, using 
				// the normal I2C channel
				imu.updateTemperature();

				t[3] = micros();
				printTiming(3);
				printIMUData();

				// Decrease buffer size for read bytes
				bytes -= 12;
			}
			else
			{
				// bytes available are more than required (12) but
				// still it doesn't contains a full message.
				// exit the while loop
				t[2] = micros();
				printTiming(2);
				break;
			}
		}
	}
	else
	{
		t[1] = micros();
		printTiming(1);
	}
}

void printTiming(int mpuRead)
{
	for (int i = 1; i <= mpuRead; i++)
	{
		if (i == 1)
			SerialPort.printf("L:0-1=%d",
				t[i] - t[i - 1]);
		else if (i == 2)
			SerialPort.printf(";(%d)%d-%d=%d",
				bytes, i, i - 1, t[i] - t[i - 1]);
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