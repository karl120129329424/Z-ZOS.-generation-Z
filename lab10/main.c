#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_READERS 10
#define ARRAY_SIZE 5

int shared_array[ARRAY_SIZE];
int write_counter = 0;

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

void* writer(void* arg) {
    while (1) {
        pthread_rwlock_wrlock(&rwlock);

        write_counter++;
        for (int i = 0; i < ARRAY_SIZE; i++) {
            shared_array[i] = write_counter;
        }

        printf("Writer (tid=%lu): wrote %d to array\n", pthread_self(), write_counter);

        pthread_rwlock_unlock(&rwlock);
        sleep(1);
    }
    return NULL;
}

void* reader(void* arg) {
    long tid = (long)arg;

    while (1) {
        pthread_rwlock_rdlock(&rwlock);

        printf("Reader (tid=%ld): [", tid);
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("%d", shared_array[i]);
            if (i < ARRAY_SIZE - 1) printf(", ");
        }
        printf("]\n");

        pthread_rwlock_unlock(&rwlock);
        usleep(500000);
    }
    return NULL;
}