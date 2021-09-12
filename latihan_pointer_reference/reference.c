#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    
    // bikin variabel
    int a = 5;
    int b = &a; // ------------> berati referensi var b itu var a


    // nyobain lihat ADDRESS si variabel A
    printf("address variabel a: %x\n", &a);
    
    // nyobain lihat NILAI si variabel A
    printf("nilai variabel a: %d\n", a);

    // nyobain lihat ADDRESS si variabel B
    printf("address variabel b: %x\n", &b);
    
    // nyobain lihat NILAI si variabel B
    printf("nilai variabel b: %d\n", b);

    return 0;
}