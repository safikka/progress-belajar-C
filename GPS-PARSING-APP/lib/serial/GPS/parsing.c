#include <glib.h>
#include <gtk/gtk.h>

#include "serial/shiki-uart-tools.h"
#include "parsing.h"
#include "ui/ui.h"
#include "db/log.h"

#include "rmc/rmc.h"
#include "gga/gga.h"
#include "gsa/gsa.h"
#include "gsv/gsv.h"

GMutex lock;
list_widget ui_widget;



gpointer baca_serial(gpointer _data_){
    
    // open si serial
    int open_serial = suart_open("/dev/ttyUSB0", 4800, 1, 30);

    
    // bikin penampung data serial
    unsigned char *wadah = NULL;
    int16_t received = 0;

    // loading data
    update_text(ui_widget.data_atas.rmc,"loading...");
    update_text(ui_widget.data_atas.gsa,"loading...");
    update_text(ui_widget.data_atas.gga,"loading...");
    update_text(ui_widget.data_atas.gsv,"loading...");

    // init mutex
    g_mutex_init(&lock);

    // mulai baca
    while(1){
        wadah = suart_read_chuncked_data(open_serial,100,(unsigned char *) "\n",1,&received);
        if(wadah != NULL){

            // Jalanin fungsi parsing RMC
            parsing_rmc(wadah);
            parsing_gga(wadah);
            parsing_gsa(wadah);
            parsing_gsv(wadah);

            free(wadah);
			wadah = NULL;
            
        }
    }
    g_mutex_clear(&lock);
    suart_close(open_serial);
    return 0;

}