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

void read_ina219() 
{

  float current_mA_1 = 0;
  float current_mA_2 = 0;

  current_mA_1 = ina219_1.getCurrent_mA();
  current_mA_2 = ina219_2.getCurrent_mA();

  Serial.print("Current_1:       "); Serial.print(current_mA_1); Serial.println(" mA");
  Serial.println("");
  Serial.print("Current_2:       "); Serial.print(current_mA_2); Serial.println(" mA");
  Serial.println("");


  delay(2000);
}

#endif