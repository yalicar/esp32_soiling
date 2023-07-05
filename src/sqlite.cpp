#include "sqlite.h"

// sqlite3 database connection
sqlite3 *db;
char *zErrMsg = 0;
int rc;
char *sql;
const char* data = "Callback function called";

// callback function
int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   Serial.printf("%s: ", (const char*)data);
   for (i = 0; i<argc; i++){
       Serial.printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   Serial.printf("\n");
   return 0;
}

// open database function
int openDb(const char *filename, sqlite3 **db) {
   int rc = sqlite3_open(filename, db);
   if (rc) {
       Serial.printf("Can't open database: %s\n", sqlite3_errmsg(*db));
       return rc;
   } else {
       Serial.printf("Opened database successfully\n");
   }
   return rc;
}

// execute SQL query
int db_exec(sqlite3 *db, const char *sql) {
    // print query to serial
    Serial.println(" ");
    Serial.println(sql);
    // execute query
    long start = micros();
    int rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
    // print error if any
    if (rc != SQLITE_OK) {
        Serial.printf("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        Serial.printf("Operation exc done successfully\n");
    }
    // manage failed query
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        // open database again
        rc = openDb("/sd/bme280.db", &db);
        if (rc) {
            Serial.println("Can't open database");
            return rc;
        }
        return rc;
    }
    // print time taken to execute query
    Serial.print(F("Time taken to execute query ms: "));
    Serial.println((micros() - start) / 1000);
    return rc;
}
