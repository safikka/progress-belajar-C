#include <glib.h>
#include <gtk/gtk.h>

#include "serial/shiki-uart-tools.h"
#include "serial-qr.h"
#include "db/log.h"
#include "ui/ui.h"

GMutex lock;
list_widget ui_widget;

gpointer baca_serial(){
    
    // open si serial
    int open_serial = suart_open("/dev/ttyUSB0", 4800, 1, 0);

    
    // bikin penampung data serial
    unsigned char *wadah = NULL;
    int16_t received = 0;

    // loading data
    ui_set_text(ROW_4,"HASIL QR");
    ui_set_text(ROW_5,"loading ...");
    

    // init mutex
    g_mutex_init(&lock);

    // mulai baca
    while(1){
        wadah = suart_read_chuncked_data(open_serial,100,(unsigned char *) "\n",1,&received);
        if(wadah != NULL){

            // Jalanin fungsi read serial
            printf("%s", wadah);


            free(wadah);
			wadah = NULL;
            
        }
    }
    g_mutex_clear(&lock);
    suart_close(open_serial);
    return 0;

}