#include "Air6814.h"

#define WU 180000

Air6814 sensor1;
Air6814 sensor2(1);

void setup() {
  Serial.begin(115200);
  Wire.begin();

  sensor1.sht30_start();
  sensor1.ads1115_config(0);

  sensor2.sht30_start();
  
  delay(WU);
}

void loop() 
{
  sensor1.readAll(ALL);
  Serial.println("Sensor 1 Data: ");
  sensor1.printData(ALL);
  delay(1000);

  sensor2.readAll(TH);
  Serial.println("Sensor 2 Data: ");
  sensor2.printData(TH);
  Serial.println("\n\n");

  delay(5000);
}