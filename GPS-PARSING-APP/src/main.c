/**
 * @file main.c
 * @author Mahdy Syafiqa Jannata (mahdy.jannata@delameta.com)
 * @brief Aplikasi Parsing Data GPS Sederhana
 * @version 0.2
 * @date 2021-07-28
 * 
 * @copyright Copyright (c) PT Delameta Bilano 2021
 * 
 */

#include <glib.h>
#include <gtk/gtk.h>
#include <sqlite3.h>

#include "serial/shiki-uart-tools.h"
#include "db/log.h"
#include "ui/ui.h"
#include "serial/GPS/parsing.h"

list_widget ui_widget;


GtkBuilder  *builder;
GMutex lock;


int main(int argc, char *argv[]){

    gtk_init(&argc, &argv);

    /**
     * @brief Fungsi untuk assign Object ke Widget buat GTK
     * 
     */
    get_object();
    ui_connect_css();


    /**
     * @brief Mulai melakukan pembacaan serial dan ambil data
     * 
     */
    GError *error = NULL;
    g_thread_try_new("baca serial",baca_serial,NULL,&error);
    if(error != NULL){
        g_print("error code: %d\n", error->code);
        g_print("error msg: %s\n", error->message);
        g_error_free(error);
        exit(EXIT_FAILURE);
    }
    
    gtk_main();
    return 0;
}