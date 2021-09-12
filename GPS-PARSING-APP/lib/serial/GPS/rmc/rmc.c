#include <gtk/gtk.h>
#include <glib.h>

#include "serial/GPS/parsing.h"
#include "ui/ui.h"
#include "db/log.h"

list_widget ui_widget;

void parsing_rmc(unsigned char *buffer){

    if(memcmp(buffer+3,"RMC",3) == 0){


        if(ui_widget.data_atas.rmc){
            update_text(ui_widget.data_atas.rmc,(gchar *)buffer);
        }

        struct log_data_t data_rmc;
        memset(&data_rmc,0,sizeof(struct log_data_t));


        // potong dulu data raw disetiap koma ","
        char *potong;
        potong = strtok((char *)buffer,",");

        // tandai tiap potongan di koma nya
        int8_t count_koma = 0;


        // variabel global
        char time_gps[14];
        char date_gps[50];
        double konversi_lat;
        double konversi_lon;
        double dir_lat;
        double dir_lon;


        while(potong != NULL){
            
            /**
             * @brief
             * brati pas tiap var potong ada isinya
             * maka kasih tambah nilai di count_koma
             * tiap ketemu koma
             * 
             * kenapa gitu? biar bisa nandain nih parameter
             * yang mau di parsing apa aja di koma ke berapa
             * gitu ges
             * 
             */
            
            count_koma++;
            // g_print("koma ada %i\n", count_koma);

            // nah tinggal disesuain deh pas koma
            // keberapa ini mau diparsing apa

            switch(count_koma){
                
                // ini parameter ke-2, berati data
                // waktu GMT
                case 2:{
                    
                    //format dari docnya
                    //hhmmss.ss

                    // seperti biasa bikin dulu wadahnya
                    char jam[3];
                    char menit[3];
                    char detik[6];
                    memset(time_gps,0,sizeof(time_gps));
                    memset(jam,0,sizeof(jam));
                    memset(menit,0,sizeof(menit));
                    memset(detik,0,sizeof(detik));

                    
                    /**
                     * @brief
                     * Copy data yang terpotong ke wadah
                     * jangan lupa kalo mau Copy sesuaikan letak data
                     * contoh: 144326.00
                     * 
                     * letak jam = potong (14)
                     * letak menit = potong+2 karena nambah 2 digit (43)
                     * letak detik = potong+4 karena nambah 4 digit (26) 
                     * 
                     */
                    memcpy(jam, potong, 2);
                    memcpy(menit, potong+2, 2);
                    memcpy(detik, potong+4, 2);


                    // data copy masukin ke var time_gps (global)
                    // biar bisa dipake di lain case
                    // di jam karena GMT, sehingga +7 sesuai WIB
                    sprintf(time_gps, "%02i:%s:%s", atoi(jam)+7, menit, detik);
                    
                    // jangan lupa di break tiap case
                    break;
                }
                
                case 3:{
                    // Position status (A = data valid, V = data invalid)

                    // state variable global   
                    data_rmc.status = potong[0];

                    char temp[40];
                    if(data_rmc.status != 'V' && data_rmc.status != 'A'){
                        data_rmc.status = 'V';
                        sprintf(temp,"%c -> Data Invalid",data_rmc.status);
                    }
                    else if(data_rmc.status == 'A'){
                        sprintf(temp,"%c -> Data Valid",data_rmc.status);
                    }
                    else{
                        sprintf(temp,"%c -> Data Invalid",data_rmc.status);
                    }

                    update_text(ui_widget.data_bawah.status,(gchar *)temp);

                    break;
                }

                case 4:{

                    // saat void
                    if(data_rmc.status == 'V'){
                        break;
                    }


                    // Latitude (DDmm.mm)
                    char lat[25];
                    char derajat[3];
                    char menit[3];
                    char belakangkoma[5];

                    memset(lat,0,sizeof(lat));
                    memset(derajat,0,sizeof(derajat));
                    memset(menit,0,sizeof(menit));
                    memset(belakangkoma,0,sizeof(belakangkoma));
                    
                    memcpy(derajat, potong, 2);
                    memcpy(menit, potong+2, 2);
                    memcpy(belakangkoma, potong+5, 4);
                    
                    konversi_lat = atof(derajat) + (((atof(belakangkoma)/10000) + atof(menit))/60.0);

                    break;
                }

                case 5:{

                    // saat void
                    if(data_rmc.status == 'V'){
                        break;
                    }
                    
                    char latdir[2];
                    memset(latdir,0,sizeof(latdir));
                    memcpy(latdir,potong,1);
                    if(memcmp(latdir,"S",1) == 0){
                        dir_lat = -1.0;
                    }
                    else if(memcmp(latdir,"N",1) == 0){
                        dir_lat = 1.0;
                    }

                    data_rmc.lat = dir_lat * konversi_lat;
                    char lattitude[20];
                    sprintf(lattitude,"%lf", data_rmc.lat);
                    
                    //Set di GUI
                    update_text(ui_widget.data_bawah.lat,lattitude);

                    break;
                }

                case 6:{
                                        
                    // saat void
                    if(data_rmc.status == 'V'){
                        break;
                    }
                    
                    // Longitude (DDDmm.mm)
                    char lon[25];
                    char derajat_lon[4];
                    char menit_lon[3];
                    char belakangkoma_lon[5];


                    memset(lon,0,sizeof(lon));
                    memset(derajat_lon,0,sizeof(derajat_lon));
                    memset(menit_lon,0,sizeof(menit_lon));
                    memset(belakangkoma_lon,0,sizeof(belakangkoma_lon));
                    
                    memcpy(derajat_lon, potong, 3);
                    memcpy(menit_lon, potong+3, 2);
                    memcpy(belakangkoma_lon, potong+6, 4);
                    
                    konversi_lon = atof(derajat_lon) + (((atof(belakangkoma_lon)/10000) + atof(menit_lon))/60.0);
                    
                    break;
                }

                case 7:{
                
                    // saat void
                    if(data_rmc.status == 'V'){
                        break;
                    }
                    
                    char londir[2];
                    memset(londir,0,sizeof(londir));
                    memcpy(londir,potong,1);
                    if(memcmp(londir,"W",1) == 0){
                        dir_lon = -1.0;
                    }
                    else if(memcmp(londir,"E",1) == 0){
                        dir_lon = 1.0;
                    }

                    data_rmc.lon = dir_lon * konversi_lon;
                    char longitude[20];
                    sprintf(longitude,"%lf", data_rmc.lon);
                                        
                    // Set di GUI
                    update_text(ui_widget.data_bawah.lon,longitude);

                    break;
                }
                
                case 8:{

                    // saat void
                    if(data_rmc.status == 'V'){
                        break;
                    }

                    // Konversi Speed Knot ke Km/h

                    int len_speed = strlen(potong) + 1;
                    char tmp[len_speed];
                    memset(tmp,0,len_speed*sizeof(char));
                    memcpy(tmp,potong,len_speed*sizeof(char));
                    
                    char speed_satuan[10];
                    char speed_koma[10];
                    memset(speed_satuan,0,sizeof(speed_satuan));
                    memset(speed_koma,0,sizeof(speed_koma));

                    int index = 0;
                    gboolean isTitik = FALSE;

                    // milah string yang ada "." nya
                    for(int i=0;i<strlen(tmp);i++){
                        if(tmp[i] == 0x2e){
                            isTitik = TRUE;
                            index= 0;
                            continue;
                        }
                        if(isTitik == FALSE){
                            speed_satuan[index] = tmp[i];
                            index++;
                        }
                        else{
                            speed_koma[index] = tmp[i];
                            index++;
                        }
                    }

                    // bagi si belakang koma
                    double speed_koma_tmp = atof(speed_koma);
                    for(int j=0; j<strlen(speed_koma); j++){
                        speed_koma_tmp /= 10;
                    }
                    
                    data_rmc.speed = (atof(speed_satuan) + speed_koma_tmp) * 1.852;
                    
                    char speed_fix[30];
                    sprintf(speed_fix,"%lf Km/h\n", data_rmc.speed);

                    // Set di GUI
                    update_text(ui_widget.data_bawah.speed,speed_fix);

                    break;
                }

                case 10:{

                    // Date parsing
                    
                    // char date_gps[30];
                    char hari[10];
                    char bulan[10];
                    char tahun[10];

                    memset(hari,0,sizeof(hari));
                    memset(bulan,0,sizeof(bulan));
                    memset(tahun,0,sizeof(tahun));
                    memcpy(hari,potong,2);
                    memcpy(bulan,potong+2,2);
                    memcpy(tahun,potong+4,2);

                    sprintf(date_gps,"%s-%s-%s", tahun, bulan, hari);
                    sprintf(data_rmc.datetime, "20%s %s", date_gps, time_gps);

                    g_print("%s", data_rmc.datetime);

                    // Set di GUI
                    update_text(ui_widget.data_bawah.datetime,data_rmc.datetime);

                    break;
                }
            }

            // JANGAN LUPA DIKASIH BREAK CUYYY!!!
            // PANIK ERROR NANGIS MAMPUS :(
            
            // BIAR TIDAK FREEZE
            while(gtk_events_pending()) gtk_main_iteration();

            potong = strtok(NULL, ",");              
            if(potong == NULL){
                simpan_db(&data_rmc);
                break;
            }
        }

    }

}