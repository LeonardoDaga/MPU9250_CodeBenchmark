# MPU9250_CodeBenchmark
A collection of codes to benchmark the speeds of different accesses to MPU9250 data

## SETUP
Test Platform: Teensy 3.6 + Drotek MPU9250

I2C speed is 400KHz (default is 100KHz)

Teensy 3.6 Settings:
- USB Powered

Drotek MPU9250 Settings:
- PULLUP switch closed
- I2C switch closed 
- Using address 0x68

### Pins
Teensy ---------> Drotek MPU9250
* L01-GND  -------> GND
* L04-INT  -------> INT
* L15-3.3V -------> VDD
* R08-SCL0 -------> SCL
* R09-SDA0 -------> SDA

Note: L=Left, R=Right, Pin numbered 1 based

## Test1: MPU9250_Basic

Example derived by the original SparkfunMPU-9250 library.
It sets sensors and then measures the "Update" and "dataReady"
execution time length.

### Results
* 193 us: imu dataReady check
* 1260 us: imu update (accel, gyros, mags, temperature)

## Test2: MPU9250_Basic_Interrupt

Example derived by the original SparkfunMPU-9250 library.
It set sensors, then the interrupt that becomes 1 to
indicate when new data is ready, then measures the time
required for checking interrupt and read data.

### Results:
* 1 us: interrupt check
* 1263 us: imu update (accel, gyros, mags, temperature)

## Test3: MPU9250_FIFO_Basic

Example derived by the original SparkfunMPU-9250 library.
This example sketch demonstrates how to use the MPU-9250's
512 byte first-in, first-out (FIFO) buffer. The FIFO can be
set to store either accelerometer and/or gyroscope (not the
magnetometer, though :( ).

Timing of the FIFO buffer handling is quite high, because
the buffer takes time to be filled and in this example 256
bytes are enough to contain more than one sample.
Results, from the point of view of the time spent to access
the FIFO, is absolutely not satisfactory.
It takes 683us to retrieve the data, but magnetometers and 
temperature are not available.

### Results:
- 370 us: fifoAvailable() >= 256
- 370 us: while (imu.fifoAvailable() > 0)
- 683 us: imu.updateFifo()

## MPU9250_FIFO_Basic_2

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

### Results:
- 370 us: fifoAvailable() >= 12
- 685 us: imu.updateFifo()
- 211 us: imu.updateTemperature();

## MPU9250_Fast_Interrupt

Same approach as the Basic_Interrupt, but now the call doesn't 
waste time in multiple calls, retrieving all the 20 bytes in 
one shot. 

### Results:
- 1 us: interrupt check
- 712 us: imu update (accel, gyros, mags, temperature)
