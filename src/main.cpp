#include <Arduino.h>
#include "time.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "bme280.h"
#include "sdmicro.h"
#include "sqlite.h"
#include "wifi_.h"
#include "ina219.h"

using namespace std;

// Prototipos de funciones
void setupWiFi();
void setupSensors();
void setupDatabase();
void createTable();
void tarea1(void* pvParameters);
void tarea2(void* pvParameters);
//void tarea3(void* pvParameters);

TaskHandle_t tarea2Handle;
const char* sql2 = "";
SemaphoreHandle_t dataReadySemaphore;
struct sensor_data {
  float temperature;
  float humidity;
  float pressure;
  float altitude;
  float current_1;
  float current_2;
  string time;
} data_sensor;

void setup() {
  Serial.begin(115200);
  setupWiFi();
  dataReadySemaphore = xSemaphoreCreateBinary();
  setupSensors();
  setupDatabase();
  createTable();
  
  xTaskCreatePinnedToCore(tarea1, "Tarea 1", 10000, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(tarea2, "Tarea 2", 10000, NULL, 1, &tarea2Handle, 0);

}

void loop() {
  // Nothing to do here
}

void tarea1(void* pvParameters) {
  TickType_t previousWakeTime = xTaskGetTickCount();
  while (true) {
    // Leer datos del sensor BME280 y del INA219
    data_sensor.temperature = bme.readTemperature();
    data_sensor.humidity = bme.readHumidity();
    data_sensor.pressure = bme.readPressure() / 100.0F;
    data_sensor.altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    data_sensor.current_1 = ina219_1.getCurrent_mA();
    data_sensor.current_2 = ina219_2.getCurrent_mA();
    data_sensor.time = string(get_time().c_str());

    // Construir la consulta SQL para insertar los datos en la base de datos
    string sql_bme_insert = "INSERT INTO bme280 (TEMPERATURE,HUMIDITY,PRESSURE,ALTITUDE,CURRENT_1,CURRENT_2,TIME) "  \
          "VALUES ("+ to_string(data_sensor.temperature) + "," \
          + to_string(data_sensor.humidity) + "," \
          + to_string(data_sensor.pressure) + "," \
          + to_string(data_sensor.altitude) + "," \
          + to_string(data_sensor.current_1) + "," \
          + to_string(data_sensor.current_2) + ",'" \
          + data_sensor.time + "');";

    // Guardar la consulta SQL en una variable global
    sql2 = sql_bme_insert.c_str();
    // Dar señal al semáforo indicando que los datos están listos
    xSemaphoreGive(dataReadySemaphore);


    // Calcular y mostrar la duración de la tarea 1
    TickType_t taskDuration = xTaskGetTickCount() - previousWakeTime;
    Serial.println("Duración de la tarea 1: " + String(taskDuration * portTICK_PERIOD_MS) + " ms" + "\n");

    // Imprimir los recursos consumidos por la tarea 1
    Serial.print("Memoria libre de la tarea 1: ");
    Serial.print(uxTaskGetStackHighWaterMark(NULL));
    Serial.println(" bytes");

    Serial.print("Stack utilizado por la tarea 1: ");
    Serial.print(uxTaskGetStackHighWaterMark(NULL) * sizeof(StackType_t));
    Serial.println(" bytes");

    // Esperar 1 segundo antes de ejecutar la próxima iteración de la tarea 1
    vTaskDelayUntil(&previousWakeTime, pdMS_TO_TICKS(1000));
  }
}

void tarea2(void* pvParameters) {
  while (true) {
    // Esperar hasta que los datos estén listos
    xSemaphoreTake(dataReadySemaphore, portMAX_DELAY);
    TickType_t startTime = xTaskGetTickCount();

    // Ejecutar la consulta SQL en la base de datos
    db_exec(db, sql2);
    // Calcular y mostrar la duración de la tarea 2
    TickType_t taskDuration = xTaskGetTickCount() - startTime;
    Serial.println("Duración de la tarea 2: " + String(taskDuration * portTICK_PERIOD_MS) + " ms" + "\n");
    // Imprimir los recursos consumidos por la tarea 2
    Serial.print("Memoria libre de la tarea 2: ");
    Serial.print(uxTaskGetStackHighWaterMark(NULL));
    Serial.println(" bytes");

    Serial.print("Stack utilizado por la tarea 2: ");
    Serial.print(uxTaskGetStackHighWaterMark(NULL) * sizeof(StackType_t));
    Serial.println(" bytes");
  }
}

// Configurar la conexión WiFi y obtener la hora del servidor NTP
void setupWiFi() {
  int estatus = setup_wifi("504", "cardenas16V");
  if (estatus != 0) {
    Serial.println("WiFi connection failed");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    esp_restart();
  }
  setup_time();
}

// Configurar los sensores
void setupSensors() {
  setup_bme280();
  setup_ina219();
}

// Configurar la base de datos
void setupDatabase() {
  setup_sd();
  openDb("/sd/bme280.db", &db);
}

// Crear la tabla en la base de datos
void createTable() {
  const char* sql = "CREATE TABLE IF NOT EXISTS bme280("  \
        "ID INTEGER PRIMARY KEY AUTOINCREMENT," \
        "TEMPERATURE           REAL    NOT NULL," \
        "HUMIDITY              REAL    NOT NULL," \
        "PRESSURE              REAL    NOT NULL," \
        "ALTITUDE              REAL    NOT NULL," \
        "CURRENT_1             REAL    NOT NULL," \
        "CURRENT_2             REAL    NOT NULL," \
        "TIME                  TEXT    NOT NULL);";

  db_exec(db, sql);
}
