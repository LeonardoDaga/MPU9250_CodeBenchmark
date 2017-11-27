/************************************************************
MPU9250_Fast_Interrupt

Same approach as the Basic_Interrupt, but now the call doesn't 
waste time in multiple calls, retrieving all the 20 bytes in 
one shot. 

Results:
- 1 us: interrupt check
- 712 us: imu update (accel, gyros, mags, temperature)
*************************************************************/
#include "MPU9250.h"

#define SerialPort SerialUSB
#define INTERRUPT_PIN 2

MPU9250_DMP imu;

void setup()
{
	pinMode(INTERRUPT_PIN, INPUT_PULLUP);
	SerialPort.begin(460800);

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

							// Likewise, the compass (magnetometer) sample rate can be
							// set using the setCompassSampleRate() function.
							// This value can range between: 1-100Hz
	imu.setCompassSampleRate(100); // Set mag rate to 100Hz

	// Use enableInterrupt() to configure the MPU-9250's 
	// interrupt output as a "data ready" indicator.
	imu.enableInterrupt();

	// The interrupt level can either be active-high or low.
	// Configure as active-low, since we'll be using the pin's
	// internal pull-up resistor.
	// Options are INT_ACTIVE_LOW or INT_ACTIVE_HIGH
	imu.setIntLevel(INT_ACTIVE_LOW);

	// The interrupt can be set to latch until data has
	// been read, or to work as a 50us pulse.
	// Use latching method -- we'll read from the sensor
	// as soon as we see the pin go LOW.
	// Options are INT_LATCHED or INT_50US_PULSE
	imu.setIntLatched(INT_LATCHED);
}

unsigned int t[4];
void loop()
{
	// The interrupt pin is pulled up using an internal pullup
	// resistor, and the MPU-9250 is configured to trigger
	// the input LOW.
	t[0] = micros();
	if (digitalRead(INTERRUPT_PIN) == LOW)
	{
		t[1] = micros();

		// Here we have the same approach as the one based on the
		// interrupt, but now the call (updateAll) invokes a 
		// function that reads all data in one call (20 bytes)
		imu.updateAll();

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
			t[1] - t[0], t[2] - t[1]);
	}
	else
	{
		if (t[1] - t[0] > 1)
		{
			SerialPort.printf("T:t0-1=%d\r\n",
				t[1] - t[0]);
		}
	}
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
	imu.getDataAll(a, g, m, T);

	SerialPort.printf("MPU:a=[% 03.2f,% 03.2f,% 03.2f]g,",
		a[X_AXIS], a[Y_AXIS], a[Z_AXIS]);
	SerialPort.printf("o=[% 03.2f,% 03.2f,% 03.2f]dps,",
		g[X_AXIS], g[Y_AXIS], g[Z_AXIS]);
	SerialPort.printf("m=[% 03.2f,% 03.2f,% 03.2f]uT,",
		m[X_AXIS], m[Y_AXIS], m[Z_AXIS]);
	SerialPort.printf("T=%2.1f,t=%lums\r\n",
		T, imu.time);
}