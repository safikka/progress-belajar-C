// header Serial
#include "shiki-uart-tools.h"

// header DB
#include "log.h"

// header GTK
#include <gtk/gtk.h>
#include <glib.h>
#include <pthread.h>
#include <stdlib.h>


struct data_rmc{
    GtkWidget   *fulldata;
    GtkWidget   *time;
    GtkWidget   *status;
    GtkWidget   *longi;
    GtkWidget   *lat;
    GtkWidget   *speed;
};

struct data_gga{
    GtkWidget   *fulldata;
    GtkWidget   *quality;
    GtkWidget   *numsat;
    GtkWidget   *hdop;
    GtkWidget   *altitude;
    GtkWidget   *geoidal;
};

struct data_gsa{
    GtkWidget   *fulldata;
    GtkWidget   *mode1;
    GtkWidget   *mode2;
    GtkWidget   *hdop;
    GtkWidget   *pdop;
    GtkWidget   *vdop;
};

struct sidebar{
    GtkWidget   *main_notebook;
    GtkWidget   *box_rmc;
    GtkWidget   *label_rmc;
    GtkWidget   *label_gga;
    GtkWidget   *label_gsa;
    GtkWidget   *label_gsv;
    GtkWidget   *box_parsing_rmc;
};


struct list_widget{
    GtkWidget   *window;
    struct data_rmc rmc;
    struct data_gga gga;
    struct data_gsa gsa;
    struct sidebar menu;
} ui_widget;

GtkBuilder  *builder;
GMutex lock;




// ------------------------------> Ini buat CSS <---------------------------------- //

void ui_init_css(){
    GtkCssProvider *css_nya;
    css_nya = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_nya,"asset/ui.css",NULL);

    // set id untuk di css
    gtk_widget_set_name(ui_widget.window, "window-main");
    gtk_widget_set_name(ui_widget.menu.main_notebook, "main_notebook");
    gtk_widget_set_name(ui_widget.menu.box_rmc, "box_rmc");
    gtk_widget_set_name(ui_widget.menu.label_rmc, "label_sidebar");
    gtk_widget_set_name(ui_widget.menu.label_gsa, "label_sidebar");
    gtk_widget_set_name(ui_widget.menu.label_gga, "label_sidebar");
    gtk_widget_set_name(ui_widget.menu.label_gsv, "label_sidebar");
    gtk_widget_set_name(ui_widget.rmc.fulldata, "label_raw");
    gtk_widget_set_name(ui_widget.gga.fulldata, "label_raw");
    gtk_widget_set_name(ui_widget.gsa.fulldata, "label_raw");
    

    gtk_style_context_add_provider_for_screen(  gdk_screen_get_default(), 
                                                GTK_STYLE_PROVIDER(css_nya),
                                                GTK_STYLE_PROVIDER_PRIORITY_USER    );
}

// ------------------------------> Ini buat CSS <---------------------------------- //





// ------------------------------> Ini buat Fungsi UI <-------------------------------- //

void btn_clicked(GtkButton *btn, gpointer data){
    g_mutex_lock(&lock);
    g_print("tombol ditekan\n");
    g_mutex_unlock(&lock);
}

// ------------------------------> Ini buat Fungsi UI <-------------------------------- //





// ------------------------------> Fungsi Mempermudah Idup <-------------------------------- //

gboolean gtk_get_object_helper(GtkWidget **_widget_ , gchar *_widget_name_,...){
	if(_widget_name_ == NULL){
		g_print("ERROR: WIDGET NAME NOT VALID ! (null)\n");
		return TRUE;
	}
	gchar *widget_name;
	va_list aptr;
	va_start(aptr, _widget_name_);
    widget_name = g_strdup_vprintf(_widget_name_,aptr);
	va_end(aptr);

	*_widget_ = GTK_WIDGET(gtk_builder_get_object(builder, widget_name));
	if(*_widget_==NULL) g_print("ERROR: FAILED GET %s\n",widget_name);
	g_free(widget_name);
	return FALSE;
}

// ------------------------------> Fungsi Mempermudah Idup <-------------------------------- //





// ------------------------------> Fungsi Parsing <-------------------------------- //

// date time
char time_gps[14];
char date_gps[30];
char datetime_fix[100];    

// status gps
char state = 'V';

// data konversi lat lon
double konversi_lat = 0.0;
double dir_lat = 0;
double lattitude_fix = 0.0;
double konversi_lon = 0.0;
int dir_lon = 0;
double longitude_fix = 0.0;

// data speed
double speed_rmc = 0.0;


void parsing_rmc(unsigned char *buffer){

    if(memcmp(buffer+3,"RMC",3) == 0){

        if(ui_widget.rmc.fulldata){
            gtk_label_set_text(GTK_LABEL(ui_widget.rmc.fulldata),(gchar *)buffer);
        }


        
        /**
         * @brief Mulai memparsing data GPRMC
         * 
         */

        
        struct log_data_t data_rmc;
        memset(&data_rmc,0,sizeof(struct log_data_t));


        // potong dulu data raw disetiap koma ","
        char *potong;
        potong = strtok((char *)buffer,",");


        // tandai tiap potongan di koma nya
        int8_t count_koma = 0;

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
            // g_print("%i\n", count_koma);

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
                    // g_print("hasil susun waktu: %s\n", time_gps);
                    
                    // jangan lupa di break tiap case
                    break;
                }
                
                case 3:{
                    // Position status (A = data valid, V = data invalid)

                    // state variable global   
                    data_rmc.status = potong[0];
                    
                    char status[40];
                    if(data_rmc.status != 'V' && data_rmc.status != 'A'){
                        data_rmc.status = 'V';
                        sprintf(status,"Status data:  %c -> Data Invalid",data_rmc.status);
                    }
                    else{
                        sprintf(status,"Status data:  %c -> Data Valid",data_rmc.status);
                    }
                    
                    // coba cetak di terminal dan gui

                    gtk_label_set_text(GTK_LABEL(ui_widget.rmc.status),(gchar *)status);


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
                    sprintf(lattitude,"Lat : %lf", data_rmc.lat);
                    // g_print("%s\n", lattitude);

                    // Set di GUI
                    gtk_label_set_text(GTK_LABEL(ui_widget.rmc.lat),lattitude);

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
                    sprintf(longitude,"Lon : %lf", data_rmc.lon);
                    // g_print("%s\n", longitude);

                    // Set di GUI
                    gtk_label_set_text(GTK_LABEL(ui_widget.rmc.longi),longitude);

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
                    sprintf(speed_fix,"Speed: %lf Km/h\n", data_rmc.speed);
                    // g_print("%s", speed_fix);

                    // Set di GUI
                    gtk_label_set_text(GTK_LABEL(ui_widget.rmc.speed),speed_fix);

                    break;
                }

                case 10:{

                    // Date parsing

                    // saat void
                    if(data_rmc.status == 'V'){
                        break;
                    }
                    
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
                    sprintf(datetime_fix, "datetime: %s\n", data_rmc.datetime);


                    g_print("%s", datetime_fix);

                    // Set di GUI
                    gtk_label_set_text(GTK_LABEL(ui_widget.rmc.time),datetime_fix);

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




// status quality
char state_gga = '0';


void parsing_gga(unsigned char *buffer){

    if(memcmp(buffer+3,"GGA",3) == 0){
        if(ui_widget.rmc.fulldata){
            gtk_label_set_text(GTK_LABEL(ui_widget.gga.fulldata),(gchar *)buffer);
        }


        
        /**
         * @brief Mulai memparsing data GPGGA
         * 
         */

        
        // potong dulu data raw disetiap koma ","
        char *potong;
        potong = strtok((char *)buffer,",");
        // g_print("isi potong: %s\n", potong);


        // tandai tiap potongan di koma nya
        int8_t count_koma = 0;

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

            // nah tinggal disesuain deh pas koma
            // keberapa ini mau diparsing apa

            switch(count_koma){
                
                case 7:{
                
                    state_gga = potong[0];
                    
                    char status[40];
                    if(state_gga != '1' && state != '2'){
                        state = '0';
                        sprintf(status,"Status data:  %c -> Data Invalid",state_gga);
                    }
                    else if(state_gga == '1'){
                        sprintf(status,"Status data:  %c -> GPS Fix",state_gga);
                    }
                    else{
                        sprintf(status,"Status data:  %c -> DGPS Fix",state_gga);
                    }
                    
                    // coba cetak di terminal dan gui
                    gtk_label_set_text(GTK_LABEL(ui_widget.gga.quality),(gchar *)status);
                    
                    // g_print("case 7: %s\n", potong);
                    
                    break;
                }
                
                case 8:{

                    char numsat[30];
                    memset(numsat,0,sizeof(numsat));
                    sprintf(numsat,"Number satellites in use: %s\n", potong);
                    
                    // coba cetak di terminal dan gui
                    gtk_label_set_text(GTK_LABEL(ui_widget.gga.numsat),(gchar *)numsat);

                    // g_print("%s", numsat);

                    break;
                }
                
                case 9:{
                    
                    char hdop[10];
                    memset(hdop,0,sizeof(hdop));
                    sprintf(hdop,"HDOP: %s\n",potong);

                    gtk_label_set_text(GTK_LABEL(ui_widget.gga.hdop),(gchar *)hdop);

                    // g_print("case 9: %s\n", potong);

                    break;
                }

                case 10:{

                    char alt[20];
                    memset(alt,0,sizeof(alt));
                    sprintf(alt,"Altitude: %s Meters\n",potong);

                    gtk_label_set_text(GTK_LABEL(ui_widget.gga.altitude),(gchar *)alt);

                    // g_print("case 10: %s\n", potong);

                    break;
                }

                case 12:{

                    char geoidal[20];
                    memset(geoidal,0,sizeof(geoidal));
                    sprintf(geoidal,"Geoidal: %s Meters\n",potong);

                    gtk_label_set_text(GTK_LABEL(ui_widget.gga.geoidal),(gchar *)geoidal);

                    // g_print("case 12: %s\n", potong);

                    break;
                }


            }
            
            // JANGAN LUPA DIKASIH BREAK CUYYY!!!
            // PANIK ERROR NANGIS MAMPUS :(
            
            // BIAR TIDAK FREEZE
            while(gtk_events_pending()) gtk_main_iteration();

            potong = strtok(NULL, ",");              
            if(potong == NULL) break;
        }

    }

}




void parsing_gsa(unsigned char *buffer){
    
    if(memcmp(buffer+3,"GSA",3) == 0){

        if(ui_widget.gsa.fulldata){
            gtk_label_set_text(GTK_LABEL(ui_widget.gsa.fulldata),(gchar *)buffer);
        }

        g_print("%s\n",buffer);
        
        // /**
        //  * @brief Mulai memparsing data GPGGA
        //  * 
        //  */
        
        gboolean isKoma = FALSE;
        int index = 0;

        int count_koma = 0;
        int len_buffer = strlen((char *)buffer);
        char tmp[len_buffer];
        memset(tmp,0,len_buffer*sizeof(char));
        memcpy(tmp,buffer,len_buffer*sizeof(char));

        char tes[10];
        
        for(int i=0; i<len_buffer; i++){
           if(tmp[i] == 0x2C){
                isKoma = TRUE;
                index = 0;
                continue;
           }
           if(isKoma == FALSE){
               tes[index] = tmp[i];
               index++;
           }
        }

        // g_print("tes %s\n", tmp);
        g_print("total koma: %i\n", count_koma);

    }
    
}


// ------------------------------> Fungsi Parsing <-------------------------------- //





// ------------------------------> Fungsi Serial <-------------------------------- //

gpointer baca_serial(gpointer _data_){

    // open si serial
    int open_serial = suart_open("/dev/ttyUSB0", 4800, 1, 30);

    
    // bikin penampung data serial
    unsigned char *wadah = NULL;
    int16_t received = 0;

    
    // loading data
    gtk_label_set_text(GTK_LABEL(ui_widget.rmc.fulldata),"loading..");
    gtk_label_set_text(GTK_LABEL(ui_widget.gga.fulldata),"loading..");
  
    
    // init mutex
    g_mutex_init(&lock);


    // mulai baca
    while(1){
        wadah = suart_read_chuncked_data(open_serial,100,(unsigned char *) "\n",1,&received);
        if(wadah != NULL){

            g_mutex_lock(&lock);

            // Jalanin fungsi parsing RMC
            parsing_rmc(wadah);
            parsing_gga(wadah);
            // parsing_gsa(wadah);

            free(wadah);
			wadah = NULL;
            g_mutex_unlock(&lock);
        }
    }
    g_mutex_clear(&lock);
    suart_close(open_serial);
    return 0;
}

// ------------------------------> Fungsi Serial <-------------------------------- //





int main(int argc, char *argv[]){



    // ------------------------------> GTK <-------------------------------- //

    // init GTK
    gtk_init(&argc, &argv);
    builder = gtk_builder_new_from_file("asset/ui.glade");

    // konek object ke widget
    gtk_get_object_helper(&ui_widget.window,"main_window");
    gtk_get_object_helper(&ui_widget.menu.label_rmc,"label_rmc");
    gtk_get_object_helper(&ui_widget.menu.label_gga,"label_gga");
    gtk_get_object_helper(&ui_widget.menu.label_gsv,"label_gsv");
    gtk_get_object_helper(&ui_widget.menu.label_gsa,"label_gsa");
    gtk_get_object_helper(&ui_widget.menu.box_rmc,"box_rmc");
    

    // object RMC
    gtk_get_object_helper(&ui_widget.rmc.fulldata,"label_raw");
    gtk_get_object_helper(&ui_widget.rmc.status,"label_status");
    gtk_get_object_helper(&ui_widget.rmc.lat,"label_lat");
    gtk_get_object_helper(&ui_widget.rmc.longi,"label_lon");
    gtk_get_object_helper(&ui_widget.rmc.speed,"label_speed");
    gtk_get_object_helper(&ui_widget.rmc.time,"label_datetime");

    // object GGA
    gtk_get_object_helper(&ui_widget.gga.fulldata,"label_gga_raw");
    gtk_get_object_helper(&ui_widget.gga.quality,"label_gga_quality");
    gtk_get_object_helper(&ui_widget.gga.numsat,"label_gga_numsat");
    gtk_get_object_helper(&ui_widget.gga.hdop,"label_gga_hdop");
    gtk_get_object_helper(&ui_widget.gga.altitude,"label_gga_altitude");
    gtk_get_object_helper(&ui_widget.gga.geoidal,"label_gga_geoidal");

    // object GSA
    gtk_get_object_helper(&ui_widget.gsa.fulldata,"label_gsa_raw");
    gtk_get_object_helper(&ui_widget.gsa.mode1,"label_gsa_mode1");
    gtk_get_object_helper(&ui_widget.gsa.mode2,"label_gsa_mode2");
    gtk_get_object_helper(&ui_widget.gsa.pdop,"label_gsa_pdop");
    gtk_get_object_helper(&ui_widget.gsa.hdop,"label_gsa_hdop");
    gtk_get_object_helper(&ui_widget.gsa.vdop,"label_gsa_vdop");
    
    

    // konekin UI
    ui_init_css();

    gtk_builder_connect_signals(builder,NULL);
    // g_signal_connect(ui_widget.btn_rmc, "clicked", G_CALLBACK (btn_clicked),NULL);
    g_signal_connect_swapped(G_OBJECT(ui_widget.window),"destroy",G_CALLBACK(gtk_main_quit),NULL);

    g_object_unref(builder);

    // ------------------------------> GTK <-------------------------------- //




    
    // ------------------------------> Init Thread <-------------------------------- //


    // GThread *serial_read = NULL;
    GError *error = NULL;


    // ------------------------------> Thread Serial <-------------------------------- //

    
    g_thread_try_new("baca serial",baca_serial,NULL,&error);
    // munculin info error
    if(error != NULL){
        g_print("error code: %d\n", error->code);
        g_print("error msg: %s\n", error->message);
        g_error_free(error);
        exit(EXIT_FAILURE);
    }
    
    
    // ------------------------------> Thread Parsing <-------------------------------- //

   
    
    // ------------------------------> End of Thread <-------------------------------- //





    gtk_widget_show(ui_widget.window);
    gtk_main();
    return 0;

}