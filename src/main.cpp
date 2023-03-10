#include <Arduino.h>
#include <Adafruit_BME280.h>
#include <Wire.h>
#include <SPI.h>
#include "SD.h"
#include "DHT.h"
#include "time.h"
#include <string>
#include <iostream>

#include "bme280.h"
#include "sdmicro.h"
#include "sqlite.h"

using namespace std;


// set pin modes for SD card reader

// DHT22 pin
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// dht22
void setup_dht22() {
  Serial.begin(9600);
  Serial.println("");
  dht.begin();
  if (isnan(dht.readTemperature())) {
    Serial.println("Failed to read from DHT sensor!");
  }
  else {
    Serial.println("DHT22 initialized.");
  }
}



void setup() {
  // put your setup code here, to run once:
  Serial.println("");
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
// get time and hour from the internet
void get_time() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  Serial.print("Current time: ");
  Serial.println(ctime(&now));
} 
void loop() {
  // put your main code here, to run repeatedly:
  string sql_bme_insert = "INSERT INTO bme280 (TEMPERATURE,HUMIDITY,PRESSURE,ALTITUDE,TIME) "  \
        "VALUES ("+ to_string(bme.readTemperature()) + "," + to_string(bme.readHumidity()) + "," + to_string(bme.readPressure() / 100.0F) + "," + to_string(bme.readAltitude(SEALEVELPRESSURE_HPA)) + "," + to_string(time(nullptr)) + ");";
  const char* sql2 = sql_bme_insert.c_str();
  db_exec(db, sql2);
  delay(1000);

}

// insert data function into the table
void insert_data() {
  Serial.begin(9600);
  Serial.println("");
  // insert dht22 data into the table , the time is inserted automatically by the database 
  string sql = "INSERT INTO DHT222 (TEMPERATURE,HUMIDITY) VALUES (" + to_string(dht.readTemperature()) + "," + to_string(dht.readHumidity()) + ");";
  const char* sql2 = sql.c_str();
  rc = sqlite3_exec(db, sql2, callback, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.println("SQL error: ");
    sqlite3_free(zErrMsg);
  } else {
    Serial.println("Records created successfully ");
  }
}