#include <glib.h>
#include <gtk/gtk.h>
#include <pthread.h>

#include "serial/shiki-uart-tools.h"
#include "serial-qr.h"
#include "db/log.h"
#include "ui/ui.h"

static int  fdesc_qr = -1;
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
            ui_set_text(ROW_5,"%s", wadah);

            free(wadah);
			wadah = NULL;
            
        }
    }
    g_mutex_clear(&lock);
    suart_close(open_serial);
    return 0;

}

void* sqr_read(void* arg){
    int16_t received = 0;

    debug(__func__,"INFO:","QR THREAD START");
    suart_close(fdesc_qr);
    fdesc_qr = -1;
    while(fdesc_qr <= 0){
        debug(__func__,"WARNING:","QR NOT OPENED");
        fdesc_qr = suart_open("/dev/ttyUSB0", 4800, 1, 0);
        sleep(1);
    }
    suart_flush(fdesc_qr);
    unsigned char *recv = NULL;
    while(1){
        recv = suart_read_chuncked_data(fdesc_qr,100,(unsigned char *) "\n",1,&received);
        if(recv != NULL){
            ui_set_text(ROW_5,"%s",recv);
            free(recv);
			recv = NULL;
        }
    }
    suart_close(fdesc_qr);
	fdesc_qr = -1;
	debug(__func__, "WARNING:", "THREAD EXIT");
	pthread_exit(NULL);
}

int sqr_start(){
    pthread_t qr_t;
    if(pthread_create(&qr_t,NULL,sqr_read,NULL) != 0){
        debug(__func__,"ERROR:","thread failed!");
        return -1;
    }
}