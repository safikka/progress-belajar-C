#include <gtk/gtk.h>
#include <glib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#define PAJAK_PER_TAHUN_PER_KENDARAAN 1500000
#define HARGA_LISTRIK_PER_KWH 1400

// coba ditambahin MUTEX
G_LOCK_DEFINE(trit_mutex);

struct struct_A{
  gfloat total_kWH;
  gchar *nama_pemilik;
  gfloat totalTagihan;
};


typedef struct{
  gint jumlah_kendaraan;
  gchar *nama_pemilik;
  gulong totalPajak;
}struct_B;


gpointer menghitung_pajak(gpointer _data_){
  
  G_LOCK(trit_mutex);
  
  struct_B *data = (struct_B *)_data_;
  g_print("Nama pemilik : %s \n",data->nama_pemilik);
  g_print("Jumlah kendaraan : %d \n",data->jumlah_kendaraan);
  
  g_print("Menghitung pajak...\n");
  g_usleep(5000000);
  
  data->totalPajak = data->jumlah_kendaraan * PAJAK_PER_TAHUN_PER_KENDARAAN;
  g_print("Total pajak terhitung : %ld \n\n",data->totalPajak);

  g_print("+---------------------------------------------+\n");
  g_print("| Total pajak tertagih : %-20ld |\n",data->totalPajak);
  g_print("+---------------------------------------------+\n");

  G_UNLOCK(trit_mutex);
  g_usleep(1);
  return 0;
}

gpointer menghitung_tagihan_listrik(gpointer _data_){
  
  G_LOCK(trit_mutex);
  
  struct struct_A *data = (struct struct_A *)_data_;
  g_print("Nama pemilik : %s \n",data->nama_pemilik);
  g_print("Total KWH : %.2f \n",data->total_kWH);
  
  g_print("Menghitung tagihan...\n");
  g_usleep(5000000);
  
  data->totalTagihan = data->total_kWH * HARGA_LISTRIK_PER_KWH;
  g_print("Tagihan listrik terhitung : %.2f \n\n",data->totalTagihan);

  g_print("+---------------------------------------------+\n");
  g_print("| Total tagihan Listrik : %-19.2lf |\n",data->totalTagihan);
  g_print("+---------------------------------------------+\n");

  G_UNLOCK(trit_mutex);
  g_usleep(1);
  return 0;
}

int main(int argc, char *argv[])
{  

  // ----------------------------------------------------------------------------------------------- //
  // ------------------------------> User bisa milih mau ngitung apa <------------------------------ //
  // ----------------------------------------------------------------------------------------------- //

  // if(argv[1] != NULL && argv[2] != NULL){
  //   if(atoi(argv[2]) == 0){
  //     // g_print("%s listrik\n", argv[1]);

  //     struct struct_A tagihanListrik;
  //     memset(&tagihanListrik,0,sizeof(struct struct_A));
  //     tagihanListrik.total_kWH = atof(argv[3]);
  //     tagihanListrik.nama_pemilik = argv[1];
  //     menghitung_tagihan_listrik(&tagihanListrik);
  //     g_print("+---------------------------------------------+\n");
  //     g_print("| Total tagihan Listrik : %-19.2lf |\n",tagihanListrik.totalTagihan);
  //     g_print("+---------------------------------------------+\n");

  //   }
  //   else if(atoi(argv[2]) == 1){
  //     // g_print("%s kendaraan\n", argv[1]);

  //     struct_B kendaraan;
  //     memset(&kendaraan,0,sizeof(struct_B));
  //     kendaraan.jumlah_kendaraan = atoi(argv[3]);
  //     kendaraan.nama_pemilik = argv[1];
  //     menghitung_pajak(&kendaraan);
  //     g_print("+---------------------------------------------+\n");
  //     g_print("| Total pajak tertagih : %-20ld |\n",kendaraan.totalPajak);
  //     g_print("+---------------------------------------------+\n");

  //   }
  // }
  // else{
  //   g_print("masukan kurang/salah, coba input <nama> <listrik = 0/kendaraan = 1> <kWh/jumlah kendaraan>\n");
  // }



  // ----------------------------------------------------------------------------------------------- //
  // ----------------------------> Cobain pake mutex jadi ngitung duaan <--------------------------- //
  // ----------------------------------------------------------------------------------------------- //

  if(argv[1] != NULL && argv[2] != NULL){
    
      // inisialisasi trit
      GThread *g_trit1 = NULL, *g_trit2 = NULL;
      GError *error = NULL;

      // Ngitung tagihan listrik

      struct struct_A tagihanListrik;
      memset(&tagihanListrik,0,sizeof(struct struct_A));
      tagihanListrik.total_kWH = atof(argv[2]);
      tagihanListrik.nama_pemilik = argv[1];
      g_trit1 = g_thread_try_new("tagihan listrik",menghitung_tagihan_listrik,&tagihanListrik,&error);

      // Ngitung tagihan pajak kendaraan

      struct_B kendaraan;
      memset(&kendaraan,0,sizeof(struct_B));
      kendaraan.jumlah_kendaraan = atoi(argv[3]);
      kendaraan.nama_pemilik = argv[1];
      g_trit2 = g_thread_try_new("tagihan pajak",menghitung_pajak,&kendaraan,&error);

      
      // munculin info error
      if(error != NULL){
          g_print("error code: %d\n", error->code);
          g_print("error msg: %s\n", error->message);
          g_error_free(error);
      }

      g_thread_join(g_trit1);
      g_thread_join(g_trit2);
  }
  else{
    g_print("masukan kurang/salah, coba input <nama> <tagihan listrik> <jumlah kendaraan>\n");
  }

  exit(EXIT_SUCCESS);
}