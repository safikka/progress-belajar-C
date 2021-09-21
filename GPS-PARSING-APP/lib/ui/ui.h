#ifndef UI_H
#define UI_H

#include <glib.h>
#include <gtk/gtk.h>
#define LBL_ROW_LEN 100

typedef enum{
	ROW_1=0,
	ROW_2,
	ROW_3,
	ROW_4,
	ROW_5,
	ROW_6,
}UI_ROW;

typedef enum{
    INTERNET_ON  = 0x00,
    INTERNET_OFF = 0x01,
    DATABASE_ON  = 0x02,
    DATABASE_OFF = 0x03,
    INIT         = 0xFF,
}UI_SET_ICON;

typedef struct{
    GtkWidget *window;
    GtkWidget *label_date;
    GtkWidget *label_time;
    GtkWidget *icon_internet;
    GtkWidget *icon_reader;
    GtkWidget *icon_gps;
    GtkWidget *icon_produk;
    GtkWidget *label_row1;
    GtkWidget *label_row3;
    GtkWidget *label_row4;
    GtkWidget *label_row5;
    GtkWidget *label_row6;
    GtkWidget *label_row7;
    GtkWidget *label_version;
    GtkWidget *image_qr;
    GtkWidget *header;
    GtkWidget *header2;
} list_widget;

char lbl_row1[LBL_ROW_LEN];
char lbl_row3[LBL_ROW_LEN];
char lbl_row4[LBL_ROW_LEN];
char lbl_row5[LBL_ROW_LEN];
char lbl_row6[LBL_ROW_LEN];
char lbl_row7[LBL_ROW_LEN];
char lbl_in_data[LBL_ROW_LEN];
char lbl_version[15];

void get_object();
void ui_connect_css();
int gui_init();
void ui_set_text(UI_ROW __row__,char *__text__,...);
void update_text(list_widget *_widget_, char *_text_);

#endif