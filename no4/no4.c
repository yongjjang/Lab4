//
// Created by tt on 19. 12. 15..
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 3
pthread_mutex_t mutex;
int sum;

pthread_t    tid[5];
pthread_mutex_t    mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t    cond = PTHREAD_COND_INITIALIZER;


void *child_thread(void *arg) {
    while (1) {
        //pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);
        printf("%d 번 째 자식 프로세스에서 메시지 수신함!\n", *((int *) arg));
        pthread_mutex_unlock(&mutex);
    }
}

void *parent_thread(void *arg){
    while(1){
        sleep(2);
        pthread_cond_broadcast(&cond);
        printf("\n메세지 전송 요청이 호출되었음!\n\n");
    }
}


void main() {
    int i;
    int id[5];
    int caller = 0;

    pthread_create(&tid[0], NULL, parent_thread, (void*)&id[0]);
    for(i = 1; i < 5; i++){
        id[i] = i;
        pthread_create(&tid[i],NULL,child_thread,(void*)&id[i]);
    }

    for(i = 1; i < 5 ; i++){
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}
