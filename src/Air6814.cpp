/*
*Copyright (c) 2025, Freshtech Co., Ltd. (주식회사 프레쉬테크)
*
*Permission is hereby granted, free of charge, to any person obtaining a copy
*of this software and associated documentation files (the "Software"), to deal
*in the Software without restriction, including without limitation the rights
*to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*copies of the Software, and to permit persons to whom the Software is
*furnished to do so, subject to the following conditions:

**The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.

**Neither the name of Freshtech Co., Ltd. nor the names of its 
* contributors may be used to endorse or promote products derived from
* this software without specific prior written permission.

*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
*LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
*OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
*SOFTWARE.
*/

#include "Air6814.h"

bool Air6814::ads1115_config(uint8_t channel)
{
  uint16_t config = 0x8000;
  config |= (0x01 + channel) << 12;
  config |= 0x0200;
  config |= 0x0080;
  config |= 0x0003; 

  Wire.beginTransmission(0x48);
  Wire.write(0x01);
  Wire.write(config >> 8);
  Wire.write(config & 0xFF);
  uint8_t res = Wire.endTransmission() == 0;
  delay(10);
  return res;
}

bool Air6814::sht30_start()
{
  Wire.beginTransmission(0x44);
  Wire.write(0x2C);
  Wire.write(0x06);
  uint8_t res = Wire.endTransmission() == 0;
  delay(10);
  return res; 
}

bool Air6814::begin()
{
  if (this->shtOnly == 0)
  {
    if (!this->ads1115_config(0))
    {
      Serial.println("MiCS6814 INIT Failed");
      return false;
    }
  }
  if (!this->sht30_start())
  {
    Serial.println("SHT30 INIT Failed");
    return false;
  }

  return true;
}

int16_t Air6814::ads1115_read()
{
  Wire.beginTransmission(0x48);
  Wire.write(0x00);
  Wire.endTransmission();
  delay(10);
  Wire.requestFrom(0x48, 2);
  delay(10);
  return (Wire.read() << 8) | Wire.read();
}

float Air6814::ads1115_toVoltage(int16_t val)
{ return val*4.096 / 32768.0; }

float Air6814::computePPM(float rs, float r0, float A, float B)
{
  float ratio = (rs / r0);
  return pow(10, ((log10(ratio) - B) / A));
}

float Air6814::getRS(gasType gasType)
{
  int16_t reading = 0;
  float result = 0;

  ads1115_config(gasType); delay(100);
  reading = ads1115_read();
  result = RL * ((5.0 - ads1115_toVoltage(reading)) / ads1115_toVoltage(reading));

  if (gasType == NH3)
    this->nh3_rs = result;
  else if (gasType == RED)
    this->red_rs = result;
  else if (gasType == OX)
    this->ox_rs = result;
  
  return result;
}

void Air6814::rsToPPM()
{
  this->getRS(NH3);
  this->getRS(RED);
  this->getRS(OX);

  this->nh3 = computePPM(this->nh3_rs, R0_NH3, -0.48, -0.7309817977);
  this->co  = computePPM(this->red_rs, R0_RED, -0.77, 0.4635861933);
  this->ch4 = computePPM(this->red_rs, R0_RED, -0.40, 0.122109002);
  this->no2 = computePPM(this->ox_rs,  R0_OX,  -1.20, -2.772829);
}

bool Air6814::sht30_read()
{
  Wire.requestFrom(0x44, 6);

  uint8_t tMSB = Wire.read();
  uint8_t tLSB = Wire.read();
  Wire.read(); // CRC
  uint8_t hMSB = Wire.read();
  uint8_t hLSB = Wire.read();
  Wire.read(); // CRC

  uint16_t tRaw = (tMSB << 8) | tLSB;
  uint16_t hRaw = (hMSB << 8) | hLSB;

  if (tRaw < 10000 || tRaw > 50000) {
    Serial.println("Temperature out of expected range. Possibly bad read.");
    return false;
  }

  this->temp = (-45) + (175 * (tRaw / 65535.0));
  this->humidity = 100 * (hRaw / 65535.0);
  return true;
}

void Air6814::readAll(int option)
{
  sht30_start();
  delay(30);
  this->sht30_read();
  
  if (option == ALL)
  {
    this->getRS(NH3);
    this->getRS(RED);
    this->getRS(OX);

    rsToPPM();
  }
}

void Air6814::printRS(gasType gasType)
{
  if (gasType == NH3)
  {
    Serial.print("NH3: "); Serial.print(this->nh3_rs); Serial.println("Ω");
    delay(10);
  }
  else if (gasType == RED)
  {
  Serial.print("RED: "); Serial.print(this->red_rs); Serial.println("Ω");
  delay(10);
  }
  else if (gasType == OX)
  {
    Serial.print("OX : "); Serial.print(this->ox_rs);  Serial.println("Ω");
    delay(10);
  }
}

const void Air6814::printData(int option)
{
  Serial.print("Temperature: "); Serial.print(this->temp); Serial.println("°C");
  delay(10);
  Serial.print("Humidity: "); Serial.print(this->humidity); Serial.println("%RH");
  delay(10);

  if (option == ALL)
  {
    Serial.print("NH3: "); Serial.print(this->nh3); Serial.println("ppm");
    delay(10);
    Serial.print("CO : "); Serial.print(this->co);  Serial.println("ppm");
    delay(10);
    Serial.print("CH4: "); Serial.print(this->ch4); Serial.println("ppm");
    delay(10);
    Serial.print("NO2: "); Serial.print(this->no2); Serial.println("ppm");
    delay(10);
    Serial.println();
  }
}

const void Air6814::printOhm()
{
  printRS(NH3);
  printRS(RED);
  printRS(OX);
}

void Air6814::setBaseline(float nh3, float red, float ox)
{
  this->R0_NH3 = nh3;
  this->R0_RED = red;
  this->R0_OX  = ox;
}