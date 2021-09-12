#include <glib.h>
#include <gtk/gtk.h>

// #include "shiki-uart-tools.h"
#include "ui.h"

list_widget ui_widget;


GtkBuilder  *builder;
GMutex lock;


// gboolean gtk_get_object_helper(GtkWidget **_widget_ , gchar *_widget_name_,...){
// 	if(_widget_name_ == NULL){
// 		g_print("ERROR: WIDGET NAME NOT VALID ! (null)\n");
// 		return TRUE;
// 	}
// 	gchar *widget_name;
// 	va_list aptr;
// 	va_start(aptr, _widget_name_);
//     widget_name = g_strdup_vprintf(_widget_name_,aptr);
// 	va_end(aptr);

// 	*_widget_ = GTK_WIDGET(gtk_builder_get_object(builder, widget_name));
// 	if(*_widget_==NULL) g_print("ERROR: FAILED GET %s\n",widget_name);
// 	g_free(widget_name);
// 	return FALSE;
// }



int main(int argc, char *argv[]){

    // gtk_init(&argc, &argv);
    // builder = gtk_builder_new_from_file("ui.glade");

    // gtk_get_object_helper(&ui_widget.main_window,"main_window");
    // // gtk_get_object_helper(&ui_widget.main_box,"main_box");
    // // gtk_get_object_helper(&ui_widget.data_atas.box_atas,"box_atas");
    // // gtk_get_object_helper(&ui_widget.data_atas.raw_box,"raw_box");
    // // gtk_get_object_helper(&ui_widget.data_atas.rmc_box,"rmc_box");
    // gtk_get_object_helper(&ui_widget.data_atas.rmc,"label_raw_rmc");
    
    // gtk_builder_connect_signals(builder,NULL);
    // g_signal_connect_swapped(G_OBJECT(ui_widget.main_window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
    // g_object_unref(builder);

    // gtk_label_set_text(GTK_LABEL(ui_widget.data_atas.rmc),"loading..");

    // gtk_widget_show(ui_widget.main_window);
    gtk_init(&argc, &argv);
    get_object();

    gtk_main();
    return 0;
}