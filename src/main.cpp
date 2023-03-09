#include <Arduino.h>
#include <Adafruit_BME280.h>
#include <Wire.h>
#include <SPI.h>
#include <sqlite3.h>
#include "SD.h"
#include "bme280.h"
#include "DHT.h"


// set pin modes for SD card reader
// SD pin
#define SD_CS 5
// DHT22 pin
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
// sqlite3 database connection
sqlite3 *db;
char *zErrMsg = 0;
int rc;
char *sql;
const char* data = "Callback function called";

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

// callback function
static int callback(void *data, int argc, char **argv, char **azColName) {
  Serial.begin(9600);
  int i;
  fprintf(stderr, "%s: ", (const char*)data);
  for (i = 0; i < argc; i++) {
    printf("%s = %s ", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  Serial.println("");
  return 0;
}

// open database
void open_db() {
  Serial.begin(9600);
  Serial.println("");
  rc = sqlite3_open("/sd/names.db", &db);
  if (rc) {
    Serial.println("Can't open database: ");
    sqlite3_close(db);
    exit(1);
  } else {
    Serial.println("Opened database successfully ");
  }
}

// create table function that receives the table name as a parameter
void create_table() {
  Serial.begin(9600);
  Serial.println("");
  sql = "CREATE TABLE IF NOT EXISTS DHT22(" \
        "ID INTEGER PRIMARY KEY AUTOINCREMENT," \
        "TEMPERATURE REAL NOT NULL," \
        "HUMIDITY REAL NOT NULL," \
        "TIME TEXT NOT NULL);";
  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.println("SQL error: ");
    sqlite3_free(zErrMsg);
  } else {
    Serial.println("Table created successfully ");
  }
}

// insert data function into the table
void insert_data() {
  Serial.begin(9600);
  Serial.println("");
  // insert dht22 data into the table , the time is inserted automatically by the database 
  sql = "INSERT INTO DHT22 (TEMPERATURE, HUMIDITY, TIME) VALUES ("+float(dht.readTemperature())+","+float(dht.readHumidity())+",datetime('now'));";
  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.println("SQL error: ");
    sqlite3_free(zErrMsg);
  } else {
    Serial.println("Records created successfully ");
  }
}

// read data from the table and print it to the serial monitor
void read_data() {
  Serial.begin(9600);
  Serial.println("");
  sql = "SELECT * from DHT22;";
  rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
  if (rc != SQLITE_OK) {
    Serial.println("SQL error: ");
    sqlite3_free(zErrMsg);
  } else {
    Serial.println("");
    Serial.println("Operation done successfully ");
  }
}



void setup() {
  // put your setup code here, to run once:
  //setup_bme280();
  // sqlite3 database connection 

  delay(1000);
  setup_sd();
  setup_dht22();
  open_db();
  create_table();
  insert_data();
}

void loop() {
  // put your main code here, to run repeatedly:
  //read_bme280();
  //read_data();
  delay(2000);
}