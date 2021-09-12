#include <gtk/gtk.h>
#include <glib.h>

#include "serial/GPS/parsing.h"
#include "ui/ui.h"
#include "db/log.h"

list_widget ui_widget;

void parsing_gsv(unsigned char *buffer){

    if(memcmp(buffer+3,"GSV",3) == 0){


        if(ui_widget.data_atas.gsv){
            update_text(ui_widget.data_atas.gsv,(gchar *)buffer);
        }

    }

}