#ifndef SQLITE_H
#define SQLITE_H

#include <sqlite3.h>
#include <Arduino.h>

// sqlite3 database connection
extern sqlite3 *db;
extern char *zErrMsg;
extern int rc;
extern char *sql;
extern const char* data;

// callback function
int callback(void *data, int argc, char **argv, char **azColName);

// open database function
int openDb(const char *filename, sqlite3 **db);

// execute SQL query
int db_exec(sqlite3 *db, const char *sql);

#endif
