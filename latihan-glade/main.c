// header C
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// header linux
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <unistd.h>

// header GTK
#include <gtk/gtk.h>
#include <glib.h>

struct list_widget{
    GtkWidget   *window;
    GtkWidget   *g_lbl_rmc;
    GtkWidget   *g_lbl_gga;
    GtkWidget   *g_lbl_gsv;
    GtkWidget   *g_lbl_gsa;
    GtkWidget   *box_body;
    GtkWidget   *box_utama;
    GtkWidget   *box_kanan;
    GtkWidget   *box_kiri;
    GtkWidget   *g_btn;
    GtkWidget   *g_btn_show;
    GtkWidget   *g_btn_hide;
    GtkWidget   *label_tes;
} ui_widget;

GtkBuilder  *builder;

GMutex lock;

// ------------------------------> Ini buat CSS <---------------------------------- //

void ui_init_css(){
    GtkCssProvider *css_nya;
    css_nya = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_nya,"main.css",NULL);

    // set id untuk di css
    gtk_widget_set_name(ui_widget.window, "window_gan");
    gtk_widget_set_name(ui_widget.g_lbl_rmc, "label");
    gtk_widget_set_name(ui_widget.g_lbl_gga, "label");
    gtk_widget_set_name(ui_widget.g_lbl_gsv, "label");
    gtk_widget_set_name(ui_widget.g_lbl_gsa, "label");
    gtk_widget_set_name(ui_widget.box_body, "box_body");
    gtk_widget_set_name(ui_widget.box_utama, "box_utama");
    gtk_widget_set_name(ui_widget.box_kanan, "box_kanan");
    gtk_widget_set_name(ui_widget.box_kiri, "box_kiri");
    gtk_widget_set_name(ui_widget.g_btn, "btn_gan");

    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(css_nya), GTK_STYLE_PROVIDER_PRIORITY_USER);
}


void on_btn_update_clicked(GtkButton *btn, gpointer data){
    g_mutex_lock(&lock);
    g_print("tombol ditekan\n");
    g_mutex_unlock(&lock);
}

void on_btn_show(GtkButton *btn, gpointer data){
    g_mutex_lock(&lock);
    gtk_widget_show(ui_widget.label_tes);
    g_mutex_unlock(&lock);
}

void on_btn_hide(GtkButton *btn, gpointer data){
    g_mutex_lock(&lock);
    gtk_widget_hide(ui_widget.label_tes);
    g_mutex_unlock(&lock);
}

gboolean gtk_get_object_helper(GtkWidget **_widget_ , gchar *_widget_name_,...){
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

gpointer baca_serial(gpointer _data_){
    // set port
    int serial_port = open("/dev/ttyUSB0", O_RDWR);
    struct termios tty;
    if(tcgetattr(serial_port, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return 0;
    }

    // konfigurasi -----------------------------> (have no idea jujur ae)
    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); 

    tty.c_lflag = 0;
    tty.c_oflag = 0;

    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 30;

    cfsetospeed (&tty, B4800);
    cfsetispeed (&tty, B4800);

    // Simpen konfigurasi
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 0;
    }

    // baca data serial
    char wadah_bacaan[256];
    char simpen;
    int total = 0;

    memset(&wadah_bacaan, 0, sizeof(wadah_bacaan));
    gtk_label_set_text(GTK_LABEL(ui_widget.g_lbl_rmc),"loading..");
    gtk_label_set_text(GTK_LABEL(ui_widget.g_lbl_gsa),"loading..");
    gtk_label_set_text(GTK_LABEL(ui_widget.g_lbl_gga),"loading..");
    gtk_label_set_text(GTK_LABEL(ui_widget.g_lbl_gsv),"loading..");
    
    g_mutex_init(&lock);

    while(1){

        simpen = 0;
        int num_byte = read(serial_port, &simpen, 1);
        

        // --------------------------------------> error handling buat data non ASCII <------------------------------
        if(simpen != 0x0A && simpen != 0x0D){
            if(simpen < 0x20 && simpen > 0x7E){
                continue;
            }
        }
        // --------------------------------------> error handling buat data non ASCII <------------------------------

        if (num_byte < 0) {
            printf("Error reading: %s", strerror(errno));
            break;
        }

        wadah_bacaan[total] = simpen;
        total++;
        if(wadah_bacaan[total-1] == 0x0A && wadah_bacaan[total-2] == 0x0D){

            g_mutex_lock(&lock);

            if(strstr(wadah_bacaan, "RMC")){
                // printf("%s\n", wadah_bacaan);
                if(ui_widget.g_lbl_rmc){               
                  gtk_label_set_text(GTK_LABEL(ui_widget.g_lbl_rmc),wadah_bacaan);
                }
                
                // char *token;
                
                // /* get the first token */
                // token = strtok(wadah_bacaan, ",");
                
                // /* walk through other tokens */
                // while( token != NULL ) {
                //     printf( "- %s\n", token );
                    
                //     token = strtok(NULL, ",");
                // }
            }
            else if(memcmp(wadah_bacaan+3, "GGA", 3) == 0){
                // printf("%s\n", wadah_bacaan);
                if(ui_widget.g_lbl_gga){
                    gtk_label_set_text(GTK_LABEL(ui_widget.g_lbl_gga),wadah_bacaan);
                }
                
            }
            else if(strstr(wadah_bacaan, "GSV")){
                // printf("%s\n", wadah_bacaan);

                if(ui_widget.g_lbl_gsv){
                    gtk_label_set_text(GTK_LABEL(ui_widget.g_lbl_gsv),wadah_bacaan);
                }
                
            }
            else if(memcmp(wadah_bacaan+1, "GPGSA",5) == 0){
                // printf("%s\n", wadah_bacaan);

                if(ui_widget.g_lbl_gsa){
                    gtk_label_set_text(GTK_LABEL(ui_widget.g_lbl_gsa),wadah_bacaan);
                }
                
            }
            // printf("%s\n", wadah_bacaan);
            memset(wadah_bacaan,0,sizeof(wadah_bacaan));
            total=0;

            g_mutex_unlock(&lock);
        }
        
    }

    g_mutex_clear(&lock);
    
    close(serial_port);
    return 0;
}

int main(int argc, char *argv[]){
    
    // ------------------------------> Ini buat GTK <---------------------------------- //
    
    gtk_init(&argc,&argv);

    builder = gtk_builder_new_from_file("glade/main.glade");

    gtk_get_object_helper(&ui_widget.window,"window");
    gtk_get_object_helper(&ui_widget.g_btn,"btn_update");
    gtk_get_object_helper(&ui_widget.g_lbl_rmc,"label_data");
    gtk_get_object_helper(&ui_widget.g_lbl_gsa,"label_data1");
    gtk_get_object_helper(&ui_widget.g_lbl_gsv,"label_data2");
    gtk_get_object_helper(&ui_widget.g_lbl_gga,"label_data3");
    gtk_get_object_helper(&ui_widget.box_body,"box_body");
    gtk_get_object_helper(&ui_widget.box_utama,"box_utama");
    gtk_get_object_helper(&ui_widget.box_kanan,"box_kanan");
    gtk_get_object_helper(&ui_widget.box_kiri,"box_kiri");
    gtk_get_object_helper(&ui_widget.g_btn_hide,"btn_hide");
    gtk_get_object_helper(&ui_widget.g_btn_show,"btn_show");
    gtk_get_object_helper(&ui_widget.label_tes,"label_tes");

    ui_init_css();

    gtk_builder_connect_signals(builder,NULL);
    g_signal_connect(ui_widget.g_btn, "clicked", G_CALLBACK (on_btn_update_clicked),NULL);
    g_signal_connect(ui_widget.g_btn_show, "clicked", G_CALLBACK(on_btn_show),NULL);
    g_signal_connect(ui_widget.g_btn_hide, "clicked", G_CALLBACK(on_btn_hide),NULL);
    g_signal_connect_swapped(G_OBJECT(ui_widget.window),"destroy",G_CALLBACK(gtk_main_quit),NULL);

    g_object_unref(builder);

    // ------------------------------> Ini buat Serial <---------------------------------- //

    GThread *serial_read = NULL;
    GError *error = NULL;
    
    serial_read = g_thread_try_new("tagihan pajak",baca_serial,NULL,&error);

    // munculin info error
    if(error != NULL){
        g_print("error code: %d\n", error->code);
        g_print("error msg: %s\n", error->message);
        g_error_free(error);
        exit(EXIT_FAILURE);
    }

    gtk_widget_show(ui_widget.window);
    gtk_main();
    return 0;
}