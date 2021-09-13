#include <glib.h>
#include <gtk/gtk.h>
#include <pthread.h>
#include "ui.h"
#include "latihan-tools/latihan-tools.h"
#include "terminal_color.h"

#define CSS_FILE           "asset/ui.css"
#define GLADE_FILE         "asset/ui.glade"
#define VERSION            "V0.3"


UI_SET_ICON db_state;
UI_SET_ICON internet_state;
list_widget ui_widget;
GtkBuilder  *builder;
GMutex lockSetRow;
GMutex lockSetIcon;
gboolean isGuiRunning = FALSE;

gboolean ui_is_gui_running(){
    return isGuiRunning;
}

void ui_hide_mouse_pointer(GtkWidget **widget){
	GdkCursor* Cursor   = gdk_cursor_new_for_display(gdk_display_get_default(),GDK_BLANK_CURSOR);
    GdkWindow* win      = gtk_widget_get_window(*widget);
    gdk_window_set_cursor(win, Cursor);
}

void on_window_main_realize(GtkWidget *widget, gpointer *data){
	isGuiRunning = TRUE;
    ui_hide_mouse_pointer(&ui_widget.window);
}

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

static gboolean ui_load_image_helper(GtkWidget **_widget,int _width,int _height,char *_file){
	GdkPixbuf * img_loader = NULL;
	GdkPixbuf * img = NULL;
	GError * err = NULL;
	img_loader = gdk_pixbuf_new_from_file (_file, &err);
	if(!img_loader){
		if(err){
			if(err->code==2) debug(__func__,"ERROR:","[%d] FAILED TO LOAD IMAGE %s",_file);
			else printf("[%d] %s\n",err->code,err->message);
			g_error_free (err);
		}
		return TRUE;
	}
	img = gdk_pixbuf_scale_simple(img_loader, _width, _height, GDK_INTERP_BILINEAR);
	g_object_unref(img_loader);
	if(!img) return TRUE;
	gtk_image_set_from_pixbuf ((GtkImage *)*_widget, img);
	g_object_unref(img);
	return FALSE;
}


void get_object(){

    builder = gtk_builder_new_from_file(GLADE_FILE);

    gtk_get_object_helper(&ui_widget.window         , "main-window");
    gtk_get_object_helper(&ui_widget.label_date     , "lbl-tanggal");
	gtk_get_object_helper(&ui_widget.label_time     , "lbl-jam");
	gtk_get_object_helper(&ui_widget.label_row1     , "lbl-produk");
	gtk_get_object_helper(&ui_widget.label_row3     , "lbl-row3");
	gtk_get_object_helper(&ui_widget.label_row4     , "lbl-row4");
	gtk_get_object_helper(&ui_widget.label_row5     , "lbl-row5");
	gtk_get_object_helper(&ui_widget.label_row6     , "lbl-row6");
	gtk_get_object_helper(&ui_widget.label_row7     , "lbl-row7");
	gtk_get_object_helper(&ui_widget.image_qr       , "image-qr");
	gtk_get_object_helper(&ui_widget.icon_produk    , "image-logo2");
	gtk_get_object_helper(&ui_widget.icon_reader    , "image-logo");
	gtk_get_object_helper(&ui_widget.icon_gps       , "image-logo3");
	gtk_get_object_helper(&ui_widget.icon_internet  , "image-logo4");

}

static void ui_gtk_init(){
    GtkCssProvider *cssProvider;
    cssProvider = gtk_css_provider_new();
    char filename[256]={0};
    sprintf(filename,"asset/ui.css");
	gtk_css_provider_load_from_path(cssProvider, filename, NULL);

    gtk_widget_set_name(ui_widget.window,         "mainWindow");
    gtk_widget_set_name(ui_widget.label_row1,     "product_name");
    gtk_widget_set_name(ui_widget.label_row3,     "row_body");
    gtk_widget_set_name(ui_widget.label_row4,     "row_body");
    gtk_widget_set_name(ui_widget.label_row5,     "row_body");
    gtk_widget_set_name(ui_widget.label_row6,     "row_body");
    gtk_widget_set_name(ui_widget.label_row7,     "row_body");
    gtk_widget_set_name(ui_widget.label_date,     "lb_date");
    gtk_widget_set_name(ui_widget.label_time,     "lb_time");

	gtk_window_set_position ((GtkWindow *) ui_widget.window, GTK_WIN_POS_CENTER_ALWAYS);
	gtk_window_fullscreen(GTK_WINDOW(ui_widget.window));

    // gtk_label_set_text ((GtkLabel *)ui_widget.label_row1, enco_tools_boot_get_product());
    gtk_label_set_text ((GtkLabel *)ui_widget.label_row3, lbl_row3);
    gtk_label_set_text ((GtkLabel *)ui_widget.label_row4, lbl_row4);
    gtk_label_set_text ((GtkLabel *)ui_widget.label_row5, lbl_row5);
    gtk_label_set_text ((GtkLabel *)ui_widget.label_row6, lbl_row6);
    gtk_label_set_text ((GtkLabel *)ui_widget.label_row7, lbl_row7);

    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    
	ui_load_image_helper(&ui_widget.icon_reader,35,35,"asset/logo_tok.png");
    ui_load_image_helper(&ui_widget.icon_internet,35,35,"asset/gsm_off.png");
    ui_load_image_helper(&ui_widget.icon_gps,35,35,"asset/db_off.png");
}


// INI DIBIKIN THREAD MALAH GAMAU MUNCUL DAH GIMANA SI

// void ui_start(int argc, char **argv){
//     internet_state = INTERNET_ON;
//     db_state       = DATABASE_OFF;

//     debug(__func__,"INFO:","GUI START");
//     gtk_init(&argc, &argv);
//     get_object();
//     g_signal_connect_swapped(G_OBJECT(ui_widget.window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
//     gtk_builder_connect_signals(builder,NULL);
//     g_object_unref(builder);
//     gtk_widget_show(ui_widget.window);
//     gtk_main();
// }

// void *th_gui(void *arg){
// 	debug(__func__, "INFO:", "GUI thread started successfully");
//     pthread_detach(pthread_self());
// 	ui_start(0,NULL);
// 	pthread_exit(NULL);
// }

static void ui_loading_hide()
{
    gtk_widget_hide(ui_widget.image_qr);
	while(gtk_events_pending()) gtk_main_iteration();

    gtk_widget_show(ui_widget.label_row3);
	while(gtk_events_pending()) gtk_main_iteration();

    gtk_widget_show(ui_widget.label_row4);
	while(gtk_events_pending()) gtk_main_iteration();

    gtk_widget_show(ui_widget.label_row5);
	while(gtk_events_pending()) gtk_main_iteration();
}

void memset_lbl_row(){
	g_mutex_lock(&lockSetRow);
    memset(lbl_row3,0,LBL_ROW_LEN);
    memset(lbl_row4,0,LBL_ROW_LEN);
	memset(lbl_row5,0,LBL_ROW_LEN);
	memset(lbl_row6,0,LBL_ROW_LEN);
	memset(lbl_row7,0,LBL_ROW_LEN);
	usleep(50000);
    g_mutex_unlock(&lockSetRow);
}

int gui_init(int argc, char **argv){

    g_mutex_init(&lockSetRow);
    g_mutex_init(&lockSetIcon);
    strcpy(lbl_version, VERSION);
    internet_state = INTERNET_OFF;
    db_state       = DATABASE_OFF;

    // INI DIBIKIN THREAD MALAH GAMAU MUNCUL DAH GIMANA SI
    
    // pthread_t th_ui;
    // if(pthread_create(&th_ui, NULL, (void*) th_gui, NULL) != 0){
	// 	debug(__func__, "ERROR:", "GUI thread start failed!");
	// 	return 1;
	// }
	// return 0;

    debug(__func__,"INFO:","GUI START");
    gtk_init(&argc, &argv);
    get_object();
    ui_gtk_init();
    g_signal_connect_swapped(G_OBJECT(ui_widget.window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
    gtk_builder_connect_signals(builder,NULL);
    g_object_unref(builder);
    gtk_widget_show(ui_widget.window);
    gtk_main();

}