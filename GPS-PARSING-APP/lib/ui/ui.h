#ifndef UI_H
#define UI_H

#include <glib.h>
#include <gtk/gtk.h>

typedef struct{
    GtkWidget *box_atas;
    GtkWidget *raw_box;
    GtkWidget *rmc_box;
    GtkWidget *rmc;
    GtkWidget *gga;
    GtkWidget *gsa;
    GtkWidget *gsv;
}data_atas;

typedef struct{
    GtkWidget *parsing_box;
    GtkWidget *datetime;
    GtkWidget *status;
    GtkWidget *lat;
    GtkWidget *lon;
    GtkWidget *speed;
    GtkWidget *numsat;
    GtkWidget *altitude;
    GtkWidget *hdop;
}data_bawah;

typedef struct{
    GtkWidget *main_window;
    GtkWidget *main_box;
    data_atas data_atas;
    data_bawah data_bawah;
}list_widget;

void get_object();
void update_text(list_widget *_widget_, char *_text_);

#endif