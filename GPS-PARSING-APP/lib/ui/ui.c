#include <glib.h>
#include <gtk/gtk.h>
#include "ui.h"

#define CSS_FILE "asset/ui.css"
#define GLADE_FILE "asset/ui.glade"

list_widget ui_widget;
GtkBuilder  *builder;
GMutex lock;




void ui_connect_css(){
    GtkCssProvider *css_nya;
    css_nya = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_nya,CSS_FILE,NULL);

    // set id untuk di css
    gtk_widget_set_name(ui_widget.main_window, "main_window");
    
    
    // HEADER
    // set id css untuk header
    gtk_widget_set_name(ui_widget.header.box_header2, "box_header2");
    gtk_widget_set_name(ui_widget.header.label_judul2, "label_judul2");
    gtk_widget_set_name(ui_widget.header.logo_atas, "logo_atas");
    gtk_widget_set_name(ui_widget.header.raw_box, "raw_box");
    
    
    // PARSING
    // set id css untuk data parsing
    gtk_widget_set_name(ui_widget.footer.label_data_1, "label_data");
    gtk_widget_set_name(ui_widget.footer.label_data_2, "label_data");
    // set id css untuk font parsingan
    gtk_widget_set_name(ui_widget.data_bawah.altitude, "label_value_parsing");
    gtk_widget_set_name(ui_widget.data_bawah.datetime, "label_value_parsing");
    gtk_widget_set_name(ui_widget.data_bawah.hdop, "label_value_parsing");
    gtk_widget_set_name(ui_widget.data_bawah.lat, "label_value_parsing");
    gtk_widget_set_name(ui_widget.data_bawah.lon, "label_value_parsing");
    gtk_widget_set_name(ui_widget.data_bawah.numsat, "label_value_parsing");
    gtk_widget_set_name(ui_widget.data_bawah.speed, "label_value_parsing");
    gtk_widget_set_name(ui_widget.data_bawah.status, "label_value_parsing");
   
    
    gtk_style_context_add_provider_for_screen(  gdk_screen_get_default(), 
                                                GTK_STYLE_PROVIDER(css_nya),
                                                GTK_STYLE_PROVIDER_PRIORITY_USER    );
}




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

    builder = gtk_builder_new_from_file(GLADE_FILE);

    
    
    // object main
    gtk_get_object_helper(&ui_widget.main_window,"main_window");
    
    
    // object atas
    gtk_get_object_helper(&ui_widget.header.box_atas,"box_atas");
    gtk_get_object_helper(&ui_widget.header.box_header2,"box_header2");
    gtk_get_object_helper(&ui_widget.header.logo_atas,"logo_atas");
    gtk_get_object_helper(&ui_widget.header.label_judul2,"label_judul2");
    gtk_get_object_helper(&ui_widget.header.raw_box,"raw_box");
    
    // object bawah
    gtk_get_object_helper(&ui_widget.footer.parsing_box,"parsing_box");
    gtk_get_object_helper(&ui_widget.footer.label_data_1,"label_data_1");
    gtk_get_object_helper(&ui_widget.footer.label_data_2,"label_data_2");


    // object atas data
    gtk_get_object_helper(&ui_widget.data_atas.rmc,"label_raw_rmc");
    gtk_get_object_helper(&ui_widget.data_atas.gsa,"label_raw_gsa");
    gtk_get_object_helper(&ui_widget.data_atas.gga,"label_raw_gga");
    gtk_get_object_helper(&ui_widget.data_atas.gsv,"label_raw_gsv");
    
    
    // object bawah data
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