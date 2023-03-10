#include "SD.h"
#include <Arduino.h>

// define SD card chip select pin
#define SD_CS 5


 // SD card
void setup_sd() {
  Serial.begin(9600);
  Serial.println("");
  Serial.println("Initializing SD card...");
  if (SD.begin(SD_CS)) {
    Serial.println("SD card initialized.");
  } else {
    Serial.println("ERROR - SD card initialization failed!");
  }
}