#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

// bikin thread
void *tesThread(void *gatau){
	while(1){
		printf("tes doang 1\n");
	}
	return NULL;
}


int main(void){
	pthread_t trit1;
	pthread_create(&trit1, NULL, tesThread, NULL);
	pthread_join(trit1, NULL);
	exit(0);
}
