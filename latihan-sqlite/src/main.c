#include <stdio.h>
#include <sqlite3.h>

int main(int argc, char *argv[]) {
    
    sqlite3 *db;
    char *error = 0;

    int db_nya = sqlite3_open("tes.db", &db);

    if(db_nya != SQLITE_OK){
        fprintf(stderr, "Can't open database : %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        return 1;
    }

    // char *tes = NULL;
    // tes = sqlite3_mprintf("INSERT INTO gprmc VALUES(1, '%s', 'A', '-6.89948', '106.209321', '5.0098');", )

    char *create = "CREATE TABLE IF NOT EXISTS gprmc(Id INT, Time TEXT, Status TEXT, Lat TEXT, Lon TEXT, Speed TEXT);";

    char *insert = "INSERT INTO gprmc VALUES(1, '2021-07-26', 'A', '-6.89948', '106.209321', '5.0098');"
                   "INSERT INTO gprmc VALUES(1, '2021-07-26', 'A', '-6.89948', '106.209321', '5.8908');"
                   "INSERT INTO gprmc VALUES(1, '2021-07-26', 'A', '-6.89948', '106.209321', '2.0198');";

    db_nya = sqlite3_exec(db, create, 0, 0, &error);

    if(db_nya != SQLITE_OK){
        fprintf(stderr, "SQL Error: %s\n", error);
        sqlite3_free(error);
        sqlite3_close(db);

        return 1;
    }
    
    db_nya = sqlite3_exec(db, insert, 0, 0, &error);

    if(db_nya != SQLITE_OK){
        fprintf(stderr, "SQL Error: %s\n", error);
        sqlite3_free(error);
        sqlite3_close(db);

        return 1;
    }

    sqlite3_close(db);

    return 0;
}