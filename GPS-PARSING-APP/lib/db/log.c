#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "log.h"


int8_t log_open(const char *_file_,sqlite3 **_db_){
    int ret = sqlite3_open(_file_, _db_);
    if(ret != SQLITE_OK){
        printf("%s\n",sqlite3_errstr(ret));
        return SQLITE_ERROR;
    }
    return SQLITE_OK;
}

int8_t log_close(sqlite3 ** _db_){
	if (sqlite3_close(*_db_) != SQLITE_OK){
	   return -1;
	}
	_db_ = NULL;
	return 0;
}

/**
 * @brief Fungsi untuk menambahkan column menggunakan alter tabel.
 * Fungsi ini dibuat untuk menghindari error akibat adanya duplikasi nama column, 
 * yang mana jika nama column yang akan dibuat sudah ada di tabel yang dituju, maka errornya akan diabaikan.
 * Jika belum ada, maka tabel tersebut boleh ditambahkan columnnya
 * 
 * @param __db__ 
 * @param __table_name 
 * @param __col 
 */
void log_add_column(sqlite3 *__db__,char*__table_name,char *__col){
	char *cmd = NULL;
	cmd = sqlite3_mprintf("ALTER TABLE %s ADD COLUMN %s;",__table_name,__col);
  if(cmd == NULL) return;
	int ret = 0;
	char *errmsg = NULL;
	ret = sqlite3_exec(__db__, cmd, 0, 0, &errmsg);
	if (ret != SQLITE_OK){
		if(!strstr(errmsg,"duplicate column name")){
			printf("%s ",cmd);
			printf("[FAILED]\n");
			printf("[%d] %s\n\n",ret,errmsg);
		}
	}
	if(errmsg) sqlite3_free(errmsg);
	sqlite3_free(cmd);
}

int simpan_db(struct log_data_t *_data_){

    const char *namaFileDb = "dataAPP.db";
    sqlite3 *db=NULL;

    int ret = log_open(namaFileDb,&db);
    if(ret != SQLITE_OK){
        return -1;
    }

    char *perintah_db = NULL;

    perintah_db = sqlite3_mprintf("CREATE TABLE IF NOT EXISTS hasil_data("
                                "datetime TEXT,"
                                "status TEXT,"
                                "data TEXT)"
    );

    

    sqlite3_exec(db, perintah_db, 0,0,NULL);
    if(ret != SQLITE_OK){
        printf("[%d] %s\n\n",ret,sqlite3_errmsg(db));
        sqlite3_free(perintah_db);
        gps_log_close(&db);
        return -1;
    }

    

    perintah_db = sqlite3_mprintf("INSERT INTO gprmc(datetime,status,lat,lon,speed)"
                                  "VALUES('%s','%s', %s)",     _data_->datetime,
                                                                _data_->status,
                                                                _data_->read_data
    );

    

    sqlite3_exec(db, perintah_db, 0,0,NULL);
    if(ret != SQLITE_OK){
        printf("[%d] %s\n\n",ret,sqlite3_errmsg(db));
        sqlite3_free(perintah_db);
        gps_log_close(&db);
        return -1;
    }
    sqlite3_free(perintah_db);
    sqlite3_close(db);

    return 0;
}