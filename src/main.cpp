#include <Arduino.h>
#include "time.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// wifi
#include "bme280.h"
#include "sdmicro.h"
#include "sqlite.h"
#include "wifi_.h"


using namespace std;

// wifi


// define SD card chip select pin
void setup() {
  // put your setup code here, to run once:
  // wifi setup
  delay(1000);
  Serial.println("");
  setup_wifi("504", "cardenas16V");
  Serial.println("");
  setup_time();
  // setup bme280 sensor
  setup_bme280();
  setup_sd();
  openDb("/sd/bme280.db", &db);
  // crate table if not exists in database bme280.db in SD card 
  const char* sql = "CREATE TABLE IF NOT EXISTS bme280("  \
        "ID INTEGER PRIMARY KEY AUTOINCREMENT," \
        "TEMPERATURE           REAL    NOT NULL," \
        "HUMIDITY              REAL    NOT NULL," \
        "PRESSURE              REAL    NOT NULL," \
        "ALTITUDE              REAL    NOT NULL," \
        "TIME                  TEXT    NOT NULL);";
  db_exec(db, sql);
}

void loop() {
  // put your main code here, to run repeatedly:
  /*
  string sql_bme_insert = "INSERT INTO bme280 (TEMPERATURE,HUMIDITY,PRESSURE,ALTITUDE,TIME) "  \
        "VALUES ("+ to_string(bme.readTemperature()) + "," + to_string(bme.readHumidity()) + "," + to_string(bme.readPressure() / 100.0F) + "," + to_string(bme.readAltitude(SEALEVELPRESSURE_HPA)) + "," + to_string(time(nullptr)) + ");";
  const char* sql2 = sql_bme_insert.c_str();
  db_exec(db, sql2);
  delay(1000);
  */
 // print date and time 
  time_t now = time(nullptr);
  Serial.println(ctime(&now));
  delay(1000);
}
