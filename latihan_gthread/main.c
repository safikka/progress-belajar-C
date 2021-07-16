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
    GError *error = NULL;
    

    gthread = g_thread_try_new("fungsi1", trit_1, GINT_TO_POINTER(datadummy), &error);
    
    // munculin info error
    if(error != NULL){
        g_print("error code: %d\n", error->code);
        g_print("error msg: %s\n", error->message);
        g_error_free(error);
    }
    g_thread_join(gthread);
    return 0;
}