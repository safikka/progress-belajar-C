// header Serial
#include "shiki-uart-tools.h"

// header GTK
#include <gtk/gtk.h>
#include <glib.h>
#include <pthread.h>

struct label{
    GtkWidget   *fulldata;
    GtkWidget   *time;
    GtkWidget   *status;
    GtkWidget   *longi;
    GtkWidget   *lat;
    GtkWidget   *speed;
};

struct list_widget{
    GtkWidget   *window;
    struct label lbl;
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





// ------------------------------> Fungsi Serial <-------------------------------- //

gpointer baca_serial(gpointer _data_){

    // open si serial
    int open_serial = suart_open("/dev/ttyUSB0", 4800, 1, 30);

    
    // bikin penampung data serial
    unsigned char *wadah = NULL;
    int16_t received = 0;

    
    // loading data
    gtk_label_set_text(GTK_LABEL(ui_widget.lbl.fulldata),"loading..");

    
    // date time
    char time_gps[14];
    char date_gps[30];    

    
    // status gps
    char state = 'V';

    
    // data konversi lat lon
    double konversi_lat = 0.0;
    double dir_lat = 0;
    double konversi_lon = 0.0;
    int dir_lon = 0;

    
    // init mutex
    g_mutex_init(&lock);


    // mulai baca
    while(1){
        wadah = suart_read_chuncked_data(open_serial,100,(unsigned char *) "\n",1,&received);
        if(wadah != NULL){

            g_mutex_lock(&lock);

            if(memcmp(wadah+3,"RMC",3) == 0){
                if(ui_widget.lbl.fulldata){
                    gtk_label_set_text(GTK_LABEL(ui_widget.lbl.fulldata),(gchar *)wadah);
                }


                
                /**
                 * @brief Mulai memparsing data GPRMC
                 * 
                 */

                
                // potong dulu data raw disetiap koma ","
                char *potong;
                potong = strtok((char *)wadah,",");
                g_print("isi potong: %s\n", potong);


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
                            g_print("hasil susun waktu: %s\n", time_gps);
                            
                            
                            // jangan lupa di break tiap case
                            break;
                        }

                    }

                    // JANGAN LUPA DIKASIH BREAK CUYYY!!!
                    // PANIK ERROR NANGIS MAMPUS :(

                    potong = strtok(NULL, ",");              
                    if(potong == NULL) break;
                }

            }

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
    gtk_get_object_helper(&ui_widget.lbl.fulldata,"label_raw");
    
    

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