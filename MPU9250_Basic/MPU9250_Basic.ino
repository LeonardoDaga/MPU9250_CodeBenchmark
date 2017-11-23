/************************************************************
MPU9250_Basic

Example derived by the original SparkfunMPU-9250 library.
It set sensors and then measures the "Update" and "dataReady"
calls, by the time required for their execution.

Results:
- 193 us: imu dataReady check
- 1260 us: imu update (accel, gyros, mags, temperature)
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

	// Use setSensors to turn on or off MPU-9250 sensors.
	// Any of the following defines can be combined:
	// INV_XYZ_GYRO, INV_XYZ_ACCEL, INV_XYZ_COMPASS,
	// INV_X_GYRO, INV_Y_GYRO, or INV_Z_GYRO
	// Enable all sensors:
	imu.setSensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS);

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

	// Likewise, the compass (magnetometer) sample rate can be
	// set using the setCompassSampleRate() function.
	// This value can range between: 1-100Hz
	imu.setCompassSampleRate(100); // Set mag rate to 100Hz
}

unsigned int t[4];
void loop()
{
	// dataReady() checks to see if new accel/gyro data
	// is available. It will return a boolean true or false
	// (New magnetometer data cannot be checked, as the library
	//  runs that sensor in single-conversion mode.)
	t[0] = micros();
	if (imu.dataReady())
	{
		t[1] = micros();

		// Call update() to update the imu objects sensor data.
		// You can specify which sensors to update by combining
		// UPDATE_ACCEL, UPDATE_GYRO, UPDATE_COMPASS, and/or
		// UPDATE_TEMPERATURE.
		// (The update function defaults to accel, gyro, compass,
		//  so you don't have to specify these values.)
		imu.update(UPDATE_ACCEL | UPDATE_GYRO | UPDATE_COMPASS | UPDATE_TEMP);
		
		t[2] = micros();
		printTiming(true);
		printIMUData();
	}
	else
	{
		t[1] = micros();
		printTiming(false);
	}
}

void printTiming(bool mpuRead)
{
	if (mpuRead)
	{
		SerialPort.printf("T:t0-1=%d,t1-2=%d\r\n",
			t[1]-t[0], t[2]-t[1]);
	}
	else
	{
		SerialPort.printf("T:t0-1=%d\r\n",
			t[1]-t[0]);
	}
}

char output[256];
unsigned int printCount = 0;
void printIMUData(void)
{
	if (printCount++ % 100 != 0)
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

