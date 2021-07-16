#include <stdio.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>

G_LOCK_DEFINE(trit_mutex);

gpointer trit_1 (gpointer data){
    while(1){
        
        // SET LOCK
        G_LOCK(trit_mutex);
        
        g_print("ini %s dengan id %ld\n",__func__, syscall(__NR_gettid));
    
        // coba nampilin data yang di pass
        gint temp = GPOINTER_TO_INT(data);
        g_print("ini data yang diterima : %d\n", temp);
        g_print("process 10 detik ...\n");
        g_usleep(2000000);
        g_print("selesai\n");

        // SET UNLOCK
        G_UNLOCK(trit_mutex);
		g_usleep (1);

    }
}

gpointer trit_2 (gpointer data){
    while(1){
        
        // SET LOCK
        G_LOCK(trit_mutex);
        
        g_print("ini %s dengan id %ld\n",__func__, syscall(__NR_gettid));
    
        // coba nampilin data yang di pass
        gint temp = GPOINTER_TO_INT(data);
        g_print("ini data yang diterima : %d\n", temp);
        g_print("process 10 detik ...\n");
        g_usleep(2000000);
        g_print("selesai\n");

        // SET UNLOCK
        G_UNLOCK(trit_mutex);
        g_usleep (1);

    }
}

int main(){
    g_print("Program Mulai!\n");

    gint datadummy = rand();
    GThread *gthread1 = NULL, *gthread2 = NULL;
    GError *error = NULL;
    

    gthread1 = g_thread_try_new("fungsi1", trit_1, GINT_TO_POINTER(datadummy), &error);
    gthread2 = g_thread_try_new("fungsi2", trit_2, GINT_TO_POINTER(datadummy), &error);
    
    // munculin info error
    if(error != NULL){
        g_print("error code: %d\n", error->code);
        g_print("error msg: %s\n", error->message);
        g_error_free(error);
    }

    g_thread_join(gthread1);
    g_thread_join(gthread2);

    g_print("\nProgram Selesai!\n");
    return 0;
}