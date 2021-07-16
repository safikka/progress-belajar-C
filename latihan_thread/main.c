#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

// bikin thread
void *tesThread(void *gatau){
	while(1){
		long * tes = gatau;
		printf("Kelipatan 2 %ld\n", *tes);
		sleep(2);
	}
	return NULL;
}

void *tes2Thread(void *gatau){
	while(1){	
		long * tes = gatau;
		printf("Kelipatan 3 %ld\n", *tes);
		sleep(3);
	}
	return NULL;
}

void *tes3Thread(void *gatau){
	while(1){	
		long * tes = gatau;
		printf("Kelipatan 5 %ld\n", *tes);
		sleep(5);
	}
	return NULL;
}


int main(void){
	pthread_t trit1, trit2, trit3;
	pthread_create(&trit1, NULL, tesThread, (void *) &trit1);
	pthread_create(&trit2, NULL, tes2Thread, (void *) &trit2);
	pthread_create(&trit3, NULL, tes3Thread, (void *) &trit3);
	pthread_join(trit1, NULL);
	pthread_join(trit2, NULL);
	pthread_join(trit3, NULL);
	pthread_exit(0);
}
