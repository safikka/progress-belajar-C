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

// #include "serial/shiki-uart-tools.h"
#include "db/log.h"
#include "ui/ui.h"
#include "serial/QR/serial-qr.h"

list_widget ui_widget;


int main(int argc, char *argv[]){


    ui_set_text(ROW_4,"HASIL QR");
    ui_set_text(ROW_5,"loading ...");
    ui_set_text(ROW_5,"tes");
    gui_init(0,NULL);
    // baca_serial();
    return 0;

}