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