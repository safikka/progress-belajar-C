#include <glib.h>
#include <gtk/gtk.h>
#include "ui.h"

list_widget ui_widget;
GtkBuilder  *builder;
GMutex lock;


/**
 * @brief Fungsi untuk mengubah object menjadi widget
 * 
 * @param _widget_ 
 * @param _widget_name_ 
 * @param ... 
 * @return gboolean 
 */
static gboolean gtk_get_object_helper(GtkWidget **_widget_ , gchar *_widget_name_,...){
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




/**
 * @brief Fungsi untuk mengubah object menjadi widget yang dipake diluar UI.c
 * 
 */
void get_object(){

    builder = gtk_builder_new_from_file("asset/ui.glade");

    
    
    // object main
    gtk_get_object_helper(&ui_widget.main_window,"main_window");
    
    
    // object atas raw
    gtk_get_object_helper(&ui_widget.data_atas.rmc,"label_raw_rmc");
    gtk_get_object_helper(&ui_widget.data_atas.gsa,"label_raw_gsa");
    gtk_get_object_helper(&ui_widget.data_atas.gga,"label_raw_gga");
    gtk_get_object_helper(&ui_widget.data_atas.gsv,"label_raw_gsv");
    
    
    // object bawah
    gtk_get_object_helper(&ui_widget.data_bawah.datetime,"label_isi_datetime");
    gtk_get_object_helper(&ui_widget.data_bawah.status,"label_isi_status");
    gtk_get_object_helper(&ui_widget.data_bawah.lat,"label_isi_lat");
    gtk_get_object_helper(&ui_widget.data_bawah.lon,"label_isi_lon");
    gtk_get_object_helper(&ui_widget.data_bawah.speed,"label_isi_speed");
    gtk_get_object_helper(&ui_widget.data_bawah.numsat,"label_isi_numsat");
    gtk_get_object_helper(&ui_widget.data_bawah.hdop,"label_isi_hdop");
    gtk_get_object_helper(&ui_widget.data_bawah.altitude,"label_isi_altitude");

    
    gtk_builder_connect_signals(builder,NULL);
    g_signal_connect_swapped(G_OBJECT(ui_widget.main_window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
    g_object_unref(builder);

    gtk_widget_show(ui_widget.main_window);

}




/**
 * @brief Fungsi untuk mengupdate label
 * 
 * @param _widget_ 
 * @param _text_ 
 */
void update_text(list_widget *_widget_, char *_text_){

    g_mutex_lock(&lock);
    
    gtk_label_set_text(GTK_LABEL(_widget_), _text_);

    while(gtk_events_pending())
	gtk_main_iteration();
    
    g_mutex_unlock(&lock);

}