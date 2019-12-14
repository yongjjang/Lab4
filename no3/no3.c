#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

#define BUFFER_SIZE 20
#define NUMITEMS 30

typedef struct {
    int item[BUFFER_SIZE];
    int totalitems;
    int in, out;
    pthread_mutex_t mutex;
    pthread_cond_t full;
    pthread_cond_t empty;
} buffer_t;

buffer_t bb = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, 0, 0,
               PTHREAD_MUTEX_INITIALIZER,
               PTHREAD_COND_INITIALIZER,
               PTHREAD_COND_INITIALIZER};

int produce_item() {
    int item = (int) (100.0 * rand() / (RAND_MAX + 1.0));
    sleep((unsigned long) 1);
    printf("--------------------------------\n");
    printf("현재 과제하는 공대생은 %d 명입니다.\n", item);

    return item;
}

int insert_item(int item) {
    int status;
    status = pthread_mutex_lock(&bb.mutex);
    if (status != 0)
        return status;
    while (bb.totalitems >= BUFFER_SIZE && status == NULL)
        status = pthread_cond_wait(&bb.empty,
                                   &bb.mutex);
    if (status != 0) {
        pthread_mutex_unlock(&bb.mutex);
        return status;
    }
    bb.item[bb.in] = item;
    bb.in = (bb.in + 1) % BUFFER_SIZE;
    bb.totalitems++;
    if (status = pthread_cond_signal(&bb.full)) {
        pthread_mutex_unlock(&bb.mutex);
        return status;
    }
    return pthread_mutex_unlock(&bb.mutex);
}

int consume_item(int item) {
    sleep((unsigned long) 1);
    printf("\n1 초 전 과제하는 %d명의 공대생에게 커피를 지급합니다\n", item);
    printf("--------------------------------\n");
}

int remove_item(int *temp) {
    int status;
    status = pthread_mutex_lock(&bb.mutex);
    if (status != 0)
        return status;
    while (bb.totalitems <= 0 && status == NULL)
        status = pthread_cond_wait(&bb.full, &bb.mutex);
    if (status != 0) {
        pthread_mutex_unlock(&bb.mutex);
        return status;
    }
    *temp = bb.item[bb.out];
    bb.out = (bb.out + 1) % BUFFER_SIZE;
    bb.totalitems--;
    if (status = pthread_cond_signal(&bb.empty)) {
        pthread_mutex_unlock(&bb.mutex);
        return status;
    }
    return pthread_mutex_unlock(&bb.mutex);
}

void *producer(void *arg) {
    int item;
    while (1) {
        item = produce_item();
        insert_item(item);
    }
}

void *consumer(void *arg) {
    int item;
    while (1) {
        remove_item(&item);
        consume_item(item);
    }
}

void main() {
    int status;
    void *result;
    pthread_t producer_tid, consumer_tid;

    /* 쓰레드 생성 */
    status = pthread_create(&producer_tid, NULL,
                            producer, NULL);
    if (status != 0)
        perror("Create producer thread");
    status = pthread_create(&consumer_tid, NULL,
                            consumer, NULL);
    if (status != 0)
        perror("Create consumer thread");
    status = pthread_join(producer_tid, NULL);
    if (status != 0)
        perror("Join producer thread");
    status = pthread_join(consumer_tid, NULL);
    if (status != 0)
        perror("Join consumer thread");
}
