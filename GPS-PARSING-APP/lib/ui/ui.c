#include <glib.h>
#include <gtk/gtk.h>
#include <pthread.h>
#include "ui.h"
#include "latihan-tools/latihan-tools.h"
#include "latihan-tools/shiki-time-tools/shiki-time-tools.h"
#include "terminal_color.h"
#include "serial/QR/serial-qr.h"

#define CSS_FILE            "asset/ui.css"
#define GLADE_FILE          "asset/ui.glade"
#define VERSION             "V0.3"
#define ANIMATION_LOADING   "asset/loading.gif"

UI_SET_ICON db_state;
UI_SET_ICON internet_state;
list_widget ui_widget;
GtkBuilder  *builder;
GdkPixbufAnimation *animation_loading;
GMutex lockSetRow;
GMutex lockSetIcon;
int ireader = 0;
int idb     = 0;
int igsm    = 0;


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

void on_window_main_close(GtkWidget *widget, gpointer *data){
    gtk_main_quit();
}

static void ui_gtk_widget_signal_connect(){
	g_signal_connect (ui_widget.window, "destroy", (GCallback) on_window_main_close, NULL);
	g_signal_connect (ui_widget.window, "realize", (GCallback) on_window_main_realize, NULL);
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
    
    animation_loading = gdk_pixbuf_animation_new_from_file (ANIMATION_LOADING, NULL);
	ui_load_image_helper(&ui_widget.icon_reader,35,35,"asset/logo_tok.png");
    ui_load_image_helper(&ui_widget.icon_internet,35,35,"asset/gsm_off.png");
    ui_load_image_helper(&ui_widget.icon_gps,35,35,"asset/db_off.png");
}

static void ui_loading_show(){
    gtk_image_set_from_animation (GTK_IMAGE(ui_widget.image_qr), animation_loading);
	while(gtk_events_pending()) gtk_main_iteration();
  
    gtk_widget_show(ui_widget.image_qr);
	while(gtk_events_pending()) gtk_main_iteration();

    gtk_widget_hide(ui_widget.label_row3);
	while(gtk_events_pending()) gtk_main_iteration();

    gtk_widget_hide(ui_widget.label_row4);
	while(gtk_events_pending()) gtk_main_iteration();

    gtk_widget_hide(ui_widget.label_row5);
	while(gtk_events_pending()) gtk_main_iteration();
}

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

void ui_set_text(UI_ROW __row__,char *__text__,...){
	g_mutex_lock(&lockSetRow);
    char tmp[LBL_ROW_LEN];
    memset(tmp,0,LBL_ROW_LEN*sizeof(char));
    va_list aptr;
	va_start(aptr, __text__);
	vsprintf(tmp, __text__, aptr);
	va_end(aptr);
    switch((int)__row__){
		case ROW_3:
		memcpy(lbl_row3, tmp,LBL_ROW_LEN);
		break;
		case ROW_4:
		memcpy(lbl_row4, tmp,LBL_ROW_LEN);
		break;
		case ROW_5:
		memcpy(lbl_row5, tmp,LBL_ROW_LEN);
		break;
		case ROW_6:
		memcpy(lbl_row6, tmp,LBL_ROW_LEN);
		break;
		// case ROW_7:
        // memcpy(lbl_row7, tmp,LBL_ROW_LEN);
		// break;
        // case ROW_8:
        // memcpy(lbl_row8, tmp,LBL_ROW_LEN);
		// break;
	}
    g_mutex_unlock(&lockSetRow);
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

static void ui_load_icon (int8_t set_to)
{
	time_t now;
	time(&now);
	struct tm *date;
	date=localtime(&now);
	
	char buftime[25];
	memset(buftime,0,sizeof(buftime));
	strftime(buftime, sizeof(buftime), "%d %b %Y %T", date);
	
    if(set_to == 1)
    {
        if (internet_state == INTERNET_ON) 
        {	
			ui_load_image_helper(&ui_widget.icon_internet,30,30,"asset/gsm_ok_4.png");
		}
        else if (internet_state == INTERNET_OFF) 
        {
	        ui_load_image_helper(&ui_widget.icon_internet,30,30,"asset/gsm_off.png");
		}
        
        printf("%s--------GSM STATE CHANGE%s\n",KYEL,KNRM);
        igsm++;
        FILE *fp = NULL;
		fp = fopen("txt/internet_state.txt","w");
		fprintf(fp,"%d) %s GSM STATE = %d\n",igsm, buftime, internet_state); 
		fclose(fp);		
    }
    else if(set_to == 2)
    {
        if (db_state == DATABASE_ON)
        {
			ui_load_image_helper(&ui_widget.icon_gps,25,30,"asset/db_ok.png");
		}
		else
		{
			ui_load_image_helper(&ui_widget.icon_gps,25,30,"asset/db_off.png");
		}
        
        printf("%s--------DB STATE CHANGE%s\n",KYEL,KNRM);
		idb++;
        FILE *fp = NULL;
		fp = fopen("txt/db_state.txt","a");
		fprintf(fp,"%d) %s DB STATE = %d\n", idb, buftime, db_state); 
		fclose(fp);
    }
    while(gtk_events_pending()) gtk_main_iteration();
}

static void ui_lbl_dtime(){
    char tmp[14];
    stim_get_date_dash_auto(tmp,ddmmyyyy);
    gtk_label_set_text ((GtkLabel *) ui_widget.label_date, tmp);	
    stim_get_time_colon_auto(tmp);
    gtk_label_set_text ((GtkLabel *) ui_widget.label_time, tmp);
	while(gtk_events_pending()) gtk_main_iteration();
}

gboolean ui_update(gpointer not_used){

    int qr_change_state = 0;
    int qr_change_state_tsc = 0;
    char lbl_row1_tmp[LBL_ROW_LEN], lbl_row3_tmp[LBL_ROW_LEN], lbl_row4_tmp[LBL_ROW_LEN], lbl_row5_tmp[LBL_ROW_LEN], lbl_row6_tmp[LBL_ROW_LEN];
    char lbl_in_tmp[LBL_ROW_LEN],lbl_out_tmp[LBL_ROW_LEN], lbl_rowvers_tmp[LBL_ROW_LEN], lbl_gardu_tmp[LBL_ROW_LEN];
    char lbl_out_info_tmp[LBL_ROW_LEN];

    int8_t internet_state_tmp, db_state_tmp;
    internet_state_tmp = db_state_tmp = -2;
    time_t time_now, time_tmp;

    time(&time_tmp);
    while(1)
    {
        time(&time_now);
        if(time_now > time_tmp){
            ui_lbl_dtime(NULL);
            time_tmp = time_now;
        }
        if(internet_state_tmp != (int8_t)internet_state){
            ui_load_icon(1);
            internet_state_tmp = (int8_t)internet_state;
        }
        if(db_state_tmp != (int8_t)db_state){
            ui_load_icon(2);
            db_state_tmp = (int8_t)db_state;
        }
        if(strcmp(lbl_row3, lbl_row3_tmp) != 0){
                gtk_label_set_text ((GtkLabel *)ui_widget.label_row3, lbl_row3);
	            memset(lbl_row3_tmp,0,sizeof(lbl_row3_tmp));
                strcpy(lbl_row3_tmp, lbl_row3);
                // qr_change_state_tsc = 0;
                while(gtk_events_pending()) gtk_main_iteration();
        }

        if(strcmp(lbl_row4, lbl_row4_tmp) != 0){
            gtk_label_set_text ((GtkLabel *)ui_widget.label_row4, lbl_row4);
            memset(lbl_row4_tmp,0,sizeof(lbl_row4_tmp));
            strcpy(lbl_row4_tmp, lbl_row4);
            // qr_change_state_tsc = 0;
            while(gtk_events_pending()) gtk_main_iteration();
        }

        if(strcmp(lbl_row5, lbl_row5_tmp) != 0){
            gtk_label_set_text ((GtkLabel *)ui_widget.label_row5, lbl_row5);
            memset(lbl_row5_tmp,0,sizeof(lbl_row5_tmp));
            strcpy(lbl_row5_tmp, lbl_row5);
            // qr_change_state_tsc = 0;
            while(gtk_events_pending()) gtk_main_iteration();
        }
        while(gtk_events_pending()) gtk_main_iteration();
        g_usleep(100000);
    }

}

// INI DIBIKIN THREAD MALAH GAMAU MUNCUL DAH GIMANA SI

void ui_start(int argc, char **argv){
    internet_state = INTERNET_ON;
    db_state       = DATABASE_OFF;

    debug(__func__,"INFO:","GUI START");
    gtk_init(&argc, &argv);
    get_object();
    ui_gtk_widget_signal_connect();
    ui_loading_hide();
    ui_gtk_init();
    gtk_builder_connect_signals(builder,NULL);
    gtk_widget_show(ui_widget.window);
    g_object_unref(builder);
    gdk_threads_add_idle(ui_update, NULL);
    gtk_main();
}

void *th_gui(void *arg){
	debug(__func__, "INFO:", "GUI thread started successfully");
    pthread_detach(pthread_self());
	ui_start(0,NULL);
	pthread_exit(NULL);
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
	// 	exit(EXIT_FAILURE);
	// }
	// return 0;

    ui_start(0, NULL);

}