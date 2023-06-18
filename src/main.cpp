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
void tarea1(void *pvParameters);
void tarea2(void *pvParameters);
// Identificador de la tarea 2
TaskHandle_t tarea2Handle;
//global variables for sql2
const char* sql2 = "";
// Semaforo para sincronizar la activacion de la tarea 2
SemaphoreHandle_t dataReadySemaphore;



// define SD card chip select pin
void setup() {
  // put your setup code here, to run once:
  // wifi setup
  delay(1000);
  Serial.println("");
  //int estatus = setup_wifi("CLARO1_8D971B", "0s3WtDtFLq");
  int estatus = setup_wifi("504", "cardenas16V");

  // stop all processes if wifi connection fails
  if (estatus != 0) {
    Serial.println("WiFi connection failed");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    esp_restart();
  }
  Serial.println("");
  // Create semaphore
  dataReadySemaphore = xSemaphoreCreateBinary();
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

    // Crear la tarea 1 en el núcleo 0
  xTaskCreatePinnedToCore(tarea1, "Tarea 1", 10000, NULL, 1, NULL, 0);

  // Crear la tarea 2 en el núcleo 1
  xTaskCreatePinnedToCore(tarea2, "Tarea 2", 10000, NULL, 1, &tarea2Handle, 1);
}

void loop() {
  // Nothing to do here
}

void tarea1(void *pvParameters) {
  while (true) {
        // put your main code here, to run repeatedly:
    string sql_bme_insert = "INSERT INTO bme280 (TEMPERATURE,HUMIDITY,PRESSURE,ALTITUDE,TIME) "  \
          "VALUES ("+ to_string(bme.readTemperature()) + "," + to_string(bme.readHumidity()) + "," \
          + to_string(bme.readPressure() / 100.0F) + "," + to_string(bme.readAltitude(SEALEVELPRESSURE_HPA)) + "," \
          + "'" + string(get_time().c_str()) + "'" + ");";
    sql2 = sql_bme_insert.c_str();
    // Semaphore to activate task 2
    xSemaphoreGive(dataReadySemaphore);
    Serial.print("Taria #1 Núcleo: ");
    Serial.print(xPortGetCoreID());
    Serial.println("");
    Serial.println("");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void tarea2(void *pvParameters) {
  while (true) {
    xSemaphoreTake(dataReadySemaphore, portMAX_DELAY);
    // using db_exec function
    db_exec(db, sql2);
    Serial.println(get_time());
    Serial.print("Tarea #2 Núcleo: ");
    Serial.print(xPortGetCoreID());
    // print space line
    Serial.println("");
  // print date and time 
  }
}
