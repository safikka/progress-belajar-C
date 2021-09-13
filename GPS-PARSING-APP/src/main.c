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


int main(int argc, char *argv[]){

    // gtk_init(&argc, &argv);

    // get_object();
    
    // gtk_main();
    gui_init(0,NULL);
    return 0;
}