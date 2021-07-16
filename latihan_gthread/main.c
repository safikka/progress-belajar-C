#include <stdio.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>

gpointer trit_1 (gpointer data){
    g_print("ini %s dengan id %ld\n",__func__, syscall(__NR_gettid));
    
    // coba nampilin data yang di pass
    gint temp = GPOINTER_TO_INT(data);
    g_print("ini data yang diterima : %d\n", temp);
    g_usleep(2000);

    // exit
    g_thread_exit(0);
}

int main(){
    g_print("Hello World\n");
    g_print("tes doang pake gprint dari glib\n");

    gint datadummy = 10;
    GThread *gthread = NULL;
    gthread = g_thread_new("fungsi1", trit_1, GINT_TO_POINTER(datadummy));
    g_thread_join(gthread);
    return 0;
}