/*
 * DatabaseAccessor.cpp
 *
 *  Created on: Sep 29, 2020
 *      Author: joey
 */
//g++ dbTest.cpp -lsqlite3 -o dbTest.out
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string>

sqlite3 *database;

int openDatabase()
{
   int databaseConnection = sqlite3_open("users.db", &database);

   if (databaseConnection)
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(database));
   else
      fprintf(stderr, "Opened database successfully\n");


   return databaseConnection;
}

//used to print database
static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
   printf("Users:\n");
   for (int i = 0; i < argc; i++)
   {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

void createUserTable()
{
   openDatabase();

   char *errorMessage;  
   char const *table = "CREATE TABLE IF NOT EXISTS USERS(" 
         "ID INT PRIMARY KEY NOT NULL," 
         "NAME TEXT NOT NULL," 
         "HOURS INT,"
         "LASTLOGTIME TEXT," //ISO8601 format (YYYY-MM-DD HH:MM:SS.SSS)
         "ISLOGGEDIN INTEGER );"; //boolean value

   int resultCode = sqlite3_exec(database, table, callback, 0, &errorMessage);

   if(resultCode != SQLITE_OK)
   {
      fprintf(stderr, "SQL error in createUserTable: %s\nResult code: %d\n", errorMessage, resultCode);
      free(errorMessage);
   }
   else
      fprintf(stdout, "Table created successfully!\n");

   sqlite3_close(database);
}

void insertNewRecord(int id, std::string name, int hours, std::string lastLogTime, int isLoggedIn)
{
   openDatabase();

   char *errorMessage; 
   char *record = (char *)malloc(sizeof(char) * 128);
   const char *baseCommand = "INSERT INTO USERS (ID,NAME,HOURS,LASTLOGTIME,ISLOGGEDIN) VALUES (%d, '%s', %d, '%s', %d ); ";

   sprintf(record, baseCommand, 
      id, name.c_str(), hours, lastLogTime.c_str(), isLoggedIn);

   int resultCode = sqlite3_exec(database, record, callback, 0, &errorMessage);
   free(record);

   if(resultCode != SQLITE_OK)
   {
      fprintf(stderr, "SQL error in insertNewRecord: %s\nResult code: %d\n", errorMessage, resultCode);
      free(errorMessage);
   }
   else
      fprintf(stdout, "Record inserted successfully!\n");

   sqlite3_close(database);
}

void printAllRecords()
{
   openDatabase();

   char *errorMessage;  
   char const *command = "SELECT * from USERS";

   int resultCode = sqlite3_exec(database, command, callback, 0, &errorMessage);

   if(resultCode != SQLITE_OK)
   {
      fprintf(stderr, "SQL error in printAllRecords: %s\nResult code: %d\n", errorMessage, resultCode);
      free(errorMessage);
   }
   else
      fprintf(stdout, "Table printed successfully!\n");

   sqlite3_close(database);
}

void updateRecord(int id, int hours, std::string lastLogTime, int isLoggedIn)
{
   openDatabase();

   char *errorMessage; 
   char *record = (char *)malloc(sizeof(char) * 128);
   const char *baseCommand = "UPDATE USERS SET " 
                              "HOURS=%d," 
                              "LASTLOGTIME='%s'," 
                              "ISLOGGEDIN=%d " 
                              "WHERE ID = %d";
                              

   sprintf(record, baseCommand, 
      hours, lastLogTime.c_str(), isLoggedIn, id);

   int resultCode = sqlite3_exec(database, record, callback, 0, &errorMessage);
   free(record);

   if(resultCode != SQLITE_OK)
   {
      fprintf(stderr, "SQL error in updateRecord: %s\nResult code: %d\n", errorMessage, resultCode);
      free(errorMessage);
   }
   else
      fprintf(stdout, "Record updated successfully!\n");

   sqlite3_close(database);
}

void deleteRecord(int id)
{
   openDatabase();

   char *errorMessage; 
   char *record = (char *)malloc(sizeof(char) * 128);
   const char *baseCommand = "DELETE from USERS " 
                              "WHERE ID = %d";

   sprintf(record, baseCommand, id);

   int resultCode = sqlite3_exec(database, record, callback, 0, &errorMessage);
   free(record);

   if(resultCode != SQLITE_OK)
   {
      fprintf(stderr, "SQL error in deleteRecord: %s\nResult code: %d\n", errorMessage, resultCode);
      free(errorMessage);
   }
   else
      fprintf(stdout, "Record updated successfully!\n");

   sqlite3_close(database);
}

int main(int argc, char *argv[])
{
   fprintf(stdout, "Opening and creating a table...\n");

   createUserTable();
   insertNewRecord(0, "Joey", 0, "0000-00-00 00:00:00.000", 0);
   printAllRecords();
   updateRecord(0, 3, "2020-09-30 15:53:30.000", 1);
   printAllRecords();
   deleteRecord(0);
   printAllRecords();
}