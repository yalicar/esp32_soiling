#ifndef INA219_H
#define INA219_H
#include "Wire.h"
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219_1(0x41);
Adafruit_INA219 ina219_2(0x40);

void setup_ina219() 
{
  Serial.begin(115200);
  while (!Serial) {
      // will pause Zero, Leonardo, etc until serial console opens
      delay(1);
  }
  if (! ina219_1.begin()) {
    Serial.println("Failed to find INA219_1 chip");
    while (1) { delay(10); }
  }
  if (! ina219_2.begin()) {
    Serial.println("Failed to find INA219_2 chip");
    while (1) { delay(10); }
  }

  ina219_1.setCalibration_16V_400mA();
  ina219_2.setCalibration_16V_400mA(); 

  Serial.println("Measuring voltage and current with INA219 ...");
}

#endif