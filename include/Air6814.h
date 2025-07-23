/*
*Copyright (c) 2025, Freshtech Co., Ltd. (주식회사 프레쉬테크)
*
*Permission is hereby granted, free of charge, to any person obtaining a copy
*of this software and associated documentation files (the "Software"), to deal
*in the Software without restriction, including without limitation the rights to
*use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
*the Software, and to permit persons to whom the Software is furnished to do so,
*subject to the following conditions:

**The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.

**Neither the name of Freshtech Co., Ltd. nor the names of its 
* contributors may be used to endorse or promote products derived from
* this software without specific prior written permission.

*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
*CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef Air6814_H
#define Air6814_H

#include <Arduino.h>
#include <Wire.h>

#define TH 0
#define ALL 1
#define RL 56000

class Air6814
{
  public:
    /// @brief Default Constructor, default option is false, which means the sensor will measure all gas + temperature & humidity
    Air6814() : Air6814(0) {}

    /// @brief Constructor for Air6814 sensor object
    /// @param option option 0: Sensor measures all gas + temperature and humidity. Option 1: Sensor only measures temperature & humidity
    Air6814(int option)
    {
      shtOnly = option;
      temp = 0.0; humidity = 0.0;
      nh3 = 0.0; co = 0.0; ch4 = 0.0; no2 = 0.0;
    }

    ~Air6814() {}

    /// @brief begin the sensor object
    /// @return true : sensor initialisation was successful
    /// @return false : sensor initialisation failed 
    bool begin();

    /// @brief Reads all data possible from Air6814 sensor object
    /// @param option ALL : All gas + temperature & humidity
    /// @param option TH : Only temperature & humidity
    void readAll(int option);

    /// @brief Prints all sensor readings
    /// @param option ALL : All gas + temperature & humidity
    /// @param option TH : Only temperature & humidity
    const void printData(int option);

    /// @brief print the current resistance measurement from MiCS6814
    const void printOhm();

    /// @brief Set the baseline resistance for MiCS6814
    void setBaseline(float NH3, float RED, float OX);

  private:
    float R0_NH3 = 99591.49;
    float R0_RED  = 99768.25;
    float R0_OX = 12361.46;

    int shtOnly;

    float temp;
    float humidity;
    float nh3;
    float co;
    float ch4;
    float no2;

    /// @brief Initialise ads1115 for MiCS6814
    /// @param channel Different channel of MOD. 0: NH3, 1: RED, 2: OX
    /// @return true : MiCS6814 initialisation was successful
    /// @return false : MiCS6814 initialisation failed 
    bool ads1115_config(uint8_t channel);

    /// @brief Initialise SHT30 I2C sensor with 0x44 address
    /// @return true : SHT30 initialisation was successful
    /// @return false : SHT30 Initialisation failed
    bool sht30_start();

    /// @brief Read raw sensor readings from MOD of MiCS6814. Call adds1115_config() before to change the target MOD.
    /// @return Raw sensor readings from MOD
    int16_t ads1115_read();

    /// @brief Converts the raw sensor readings to Voltage
    /// @param val Raw sensor readings
    /// @return Corresponding voltage in floating point
    float ads1115_toVoltage(int16_t val);

    /// @brief Computes PPM estimates from MOD resistance
    /// @param rs MOD resistance value
    /// @param r0 Reference Resistance value (measured in clean air)
    /// @param A Computation constant A
    /// @param B Computation constant B
    /// @return Computed ppm estimates
    float computePPM(float rs, float r0, float A, float B);

    /// @brief Converts raw sensor readings to PPM
    /// @param nh3 raw sensor reading from NH3 MOD
    /// @param red raw sensor reading from RED MOD
    /// @param ox raw sensor reading from OX MOD
    void rawToPPM(float nh3, float red, float ox);

    /// @brief Reads raw temperature and humidity sensor readings from SHT30
    /// @param sensor Air6814 Sensor object
    /// @return true : Sensor reading was successful
    /// @return false : Sensor reading failed 
    bool sht30_read();
};
#endif