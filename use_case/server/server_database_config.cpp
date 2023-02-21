#include <stdio.h>
#include <sys/stat.h>
#include "server_database_config.h"

// COMPILATION: g++ -c server/server_app/server_database_config.cpp -o server/server_app/server_database_config.o
// LINKEDITON: g++ server/server_app/server_database_config.o -l sqlite3 -o server/server_app/server_database_config

// Credits to IInspectable: https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exists-using-standard-c-c11-14-17-c
inline bool verify_if_file_exists (const std::string& name) {
    struct stat buffer;   
    return (stat (name.c_str(), &buffer) == 0); 
}

/*
 * received_from_exec = data passed througth the 4th argument of sqlite3_exec
 * num_columns = number of columns (fields) in the database
 * columns_values = array of strings with fields values for the corresponding row
 * columns_names = array of strings with columns names
 */
static int callback_create_table(void* received_from_exec, int num_columns, char** columns_values, char ** columns_names) {
    
    // print data received from the sqlite3_exec call 
    printf("%s: \n", (const char*)received_from_exec);
  
    // Navigate througth all colums
    for(int i = 0; i<num_columns; i++) {

        // Print columns names and corresponding value (or print NULL instead of the value)
        printf("%s = %s\n", columns_names[i], columns_values[i] ? columns_values[i] : "NULL");
    }

    printf("\n");
    return 0;
}


int configure_database(char* name){

    char path[256];
    sprintf(path, "./use_case/database/%s", name);

    if(verify_if_file_exists(path)) {
        printf("Database file alredy exists\n");
    }

    // Open connection to database (create if it is not yet)
    sqlite3 *db;
    int ret = sqlite3_open(path, &db);
   
    if(ret) {
       printf("Can't open database: %s\n", sqlite3_errmsg(db));
       return -1;
    } else {
       printf("Opened database successfully\n");
    }
    
    // Create SQL statement for creating a table
    // CREATE TABLE TACIOT (ID INT PRIMARY KEY NOT NULL, TYPE CHAR(6) NOT NULL, ...);
    std::string create_table_sql_statement_string = 
      "CREATE TABLE TACIOT ("   \
      "ID           INT PRIMARY KEY         NOT NULL,"  \
      "TIME         CHAR(20)                NOT NULL,"  \
      "TYPE         CHAR(6)                 NOT NULL,"  \
      "PK           CHAR(8)                 NOT NULL,"  \
      "SIZE         INT                     NOT NULL,"  \
      "ENCRYPTED    TEXT                    NOT NULL    );";
    const char *create_table_sql_statement = create_table_sql_statement_string.c_str();

    // Execute SQL statetment for creating a table, passing data for callback function
    char passed_to_callback[50] = "Callback function called successfully";  
    char *error_message = 0;
    ret = sqlite3_exec(db, create_table_sql_statement, callback_create_table, passed_to_callback, &error_message);

    if(ret != SQLITE_OK ){
        printf("SQL error: %s\n", error_message);

        // Error message is allocated inside sqlite3_exec call IF ther were an error
        sqlite3_free(error_message);
        
        sqlite3_close(db);
        return -1;
    } else 
       printf("Table created successfully\n");

    // Close connection to database
    sqlite3_close(db);
    return 0;
}

int main (int agrc, char** argv) {
    return configure_database(argv[1]);
}