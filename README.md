# Freshtech Air6814 Sensor Module Library

This is the Freshtech Air6814 library allowing you to communicate with Air6814 over I2C.
Due to company policy, publicly available version only supports AVR framework.
Library for ZephyrOS is available via B2B purchasing

<img src="images/Air6814.png">

## Sensor Information
Freshtech Air6814 is an atmospheric sensor module with SGX MiCS68184 and SHT30 sensor chips.
Using 3 MODs in MiCS6814, it measures multiple gas types. This library helps conversion of those raw values into ppm estimates.
MiCS6814 is communicates through ADC, which is connected in the same I2C bus as SHT30.

SHT30 can be called at the same time as MiCS6814 but due to the nature of how MiCS6814 works, the heat generated from it influences the reading of SHT30. (Typically adds ~20°C to the temperature reading and deducts ~30RH% to relative temperature readings.

| Sensor name | Readings | I²C Addresses |
|-------------|----------|-------------- |
|MiCS6814 (Through ADC)|NH3, CO, CH4, NO2| **0x48**|
|SHT30|Temperature(°C), Relative Humidity(RH%)| **0x44**|

## Sensor Calibration
Freshtech Air6814 is calibrated with following baseline Resistance:
| MOD Type | Baseline Ω | Expected Output in ppm |
|----------|------------|------------------------|
| NH3      | 99591.49   | NH3: 0.03              |
| RED      | 99768.25   | CO: 4.0, CH4: 1.8      |
| OX       | 12361.46   | NO2: 0.0049            | 

This calibration was done in clean air at Jeju, South Korea
If you feel such calibration does not fit your environment, you can re-calibrate your sensor by:
1. Pre-heating the sensor for at least 24 hours (48 hours is recommended)
2. Take multiple resistance readings using `getRS()` and `printOhm()` method at clean air
3. Average out the data and calibrate it to acutal respective gas concentration
4. Use the resistance value for new baseline using `setBaseline()` method. 

## Installation of the library
This library can be installed using the PlatformIO Library manager:
Search `Air6814` in PlatformIO then add to your desired project,
or simply add `lib_deps = pisache/Air6814@^1.0.0` at the end of your `platformio.ini`

## Connect the sensor
Use the following pin description to connect your Air6814 to the standard I2C bus:
<img src="images/port.png" width="300px">

| *pin* | *Name* | *Description* |
|-------|:------:|---------------|
| 1 | GND | Ground |
| 2 | VDD | 5.0V |
| 3 | SCL | I2C: Serial Clock Input |
| 4 | SDA | I2C: Serial Data Input / Output |

## Quick Start
1. Install the library according to [Installation of the library](#installation-of-the-library)
2. Connect the Air6814 sensor module to your board as explained in [Connect the sensor](#connect-the-sensor)
3. Open the `Air6814Usage` sample project within your IDE:
   `File` ➔ `example` ➔ `Air6814Usage`
4. Click the `upload` button in the IDE
5. When the upload process has finished, open the `Serial Monitor` to monitor the readings. Note that the monitor speed is set to 115200.

## License
See [LICENSE](LICENSE).
