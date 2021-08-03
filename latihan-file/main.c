#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 

// char* _keyword_, char* _value_
int baca_file(char* _namafile_, char* _keyword_, char** _value_){

    
    char temp[150];
    int8_t tanda_ketemu = 0;
    bool cek = false;
    int8_t index = 0;

    
    /**
     * @brief untuk baca file maka beri perintah atau mode "r"
     * 
     */
    FILE * fpointer = fopen(_namafile_, "r");
    

    
    
    /**
     * @brief Error handling saat buka file
     * 
     */
    if(!fpointer){
        printf("Gagal buka file\n");
        return -9;
    }
    else{
        printf("File terbuka\n\n");


        memset(temp,0,sizeof(temp));

        /**
         * @brief Mulai pencarian dengan fungsi fgets
         * dimana fgets mencari tiap baris line
         * 
         */
        while(fgets(temp,50,fpointer) != NULL){

            if((strstr(temp,_keyword_)) != NULL){
                

                // for(int i=0; i<strlen(temp); i++){
                //     if(temp[i] == '='){
                //         cek = true;
                //         index=0;
                //         continue;
                //     }
                //     if(cek != false){
                //         temp_value[index] = temp[i];
                //         index++;
                //     }
                // }

                // printf("%s\n", temp_value);
                int posisi = strlen(_keyword_)+1;
                printf("%d\n", posisi);
                
                char temp_value[50];
                memset(temp_value,0,50);
                
                memcpy(temp_value,temp+posisi,strlen(temp)-posisi);
                printf("ini value : %s\n", temp_value);
                
                memcpy(*_value_,temp_value,strlen(temp_value));


                memset(temp,0,sizeof(temp));
                /**
                 * @brief Kasih penanda kalo ketemu biar kalo
                 * ga ditemukan by keyword nanti tanda_ketemu = 0
                 * 
                 */
                tanda_ketemu++;
            }
            memset(temp,0,sizeof(temp));

        }
        if(!tanda_ketemu){
            printf("Keyword tidak ditemukan\n");
            return 1;
        }
        
        fflush(fpointer);
        fclose(fpointer);
    }
}


int main(int argc, char* argv[]){

    if(argc < 2){
        printf("argumen kurang! berikut parameternya ./tes <dirfile> <keyword>\ncontoh:\n\n./tes conf/db.conf PCDS\n\n");
        return -1;
    }

    char *value;

    if(!baca_file(argv[1],argv[2],&value)){
        printf("%s",value);
    }

    return 0;

}