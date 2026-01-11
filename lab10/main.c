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

int main() {
    pthread_t writer_thread;
    pthread_t reader_threads[NUM_READERS];

    if (pthread_create(&writer_thread, NULL, writer, NULL) != 0) {
        perror("Ошибка: не удалось создать пишущий поток");
        exit(EXIT_FAILURE);
    }

    for (long i = 0; i < NUM_READERS; i++) {
        if (pthread_create(&reader_threads[i], NULL, reader, (void*)i) != 0) {
            perror("Ошибка: не удалось создать читающий поток");
            exit(EXIT_FAILURE);
        }
    }

    printf("Программа запущена. Нажмите Ctrl+C для остановки.\n");

    pthread_join(writer_thread, NULL);
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(reader_threads[i], NULL);
    }

    pthread_rwlock_destroy(&rwlock);
    return 0;
}