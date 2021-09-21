#ifndef LOG_H
#define LOG_H

struct log_data_t{
    char datetime[21];
    char* status;
    char* read_data;
};

int simpan_db(struct log_data_t *_data_);

#endif