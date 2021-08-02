#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// char* _keyword_, char* _value_
int baca_file(char* _namafile_, char* _keyword_, char* _value_){

    
    char temp[50];
    int8_t tanda_ketemu = 0;

    
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



        /**
         * @brief Mulai pencarian dengan fungsi fgets
         * dimana fgets mencari tiap baris line
         * 
         */
        while(fgets(temp,50,fpointer) != NULL){

            if((strstr(temp,_keyword_)) != NULL){
                
                if((strstr(temp,_value_)) != NULL){

                    printf("Pencarian ditemukan:\n");
                    printf("%s\n", temp);

                }
                
                /**
                 * @brief Kasih penanda kalo ketemu biar kalo
                 * ga ditemukan by keyword nanti tanda_ketemu = 0
                 * 
                 */
                tanda_ketemu++;

            }
        }
        if(!tanda_ketemu){
            printf("Keyword tidak ditemukan\n");
            return 1;
        }

        fclose(fpointer);

    }

}


int main(int argc, char* argv[]){

    if(argc < 2){
        printf("argumen kurang! berikut parameternya ./tes <dirfile> <keyword>\ncontoh:\n\n./tes conf/db.conf PCDS\n\n");
        return -1;
    }
    baca_file(argv[1],argv[2],argv[3]);
    return 0;

}