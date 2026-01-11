#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define ARRAY_SIZE 10
#define MAX_ITERATIONS 5

// Общий буфер и мьютекс
char shared_array[ARRAY_SIZE];
pthread_mutex_t array_mutex = PTHREAD_MUTEX_INITIALIZER;

void* writer_thread(void* arg) {
    (void)arg; // подавляем предупреждение
    int counter = 0;
    while (counter < MAX_ITERATIONS) {
        pthread_mutex_lock(&array_mutex);
        snprintf(shared_array, ARRAY_SIZE, "%d", counter);
        pthread_mutex_unlock(&array_mutex);

        printf("Писатель записал: %s\n", shared_array);
        usleep(1000000); // 1 секунда
        counter++;
    }
    return NULL;
}

void* reader_thread(void* arg) {
    (void)arg;
    pthread_t tid = pthread_self();
    int counter = 0;
    while (counter < MAX_ITERATIONS) {
        pthread_mutex_lock(&array_mutex);
        char local_copy[ARRAY_SIZE];
        strncpy(local_copy, shared_array, ARRAY_SIZE);
        pthread_mutex_unlock(&array_mutex);

        printf("Читатель [TID: %p] прочитал: %s\n", (void*)tid, local_copy);
        usleep(500000); // 0.5 секунды
        counter++;
    }
    return NULL;
}

int main() {
    pthread_t writer, reader;

    if (pthread_create(&writer, NULL, writer_thread, NULL) != 0) {
        perror("Ошибка создания потока писателя");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&reader, NULL, reader_thread, NULL) != 0) {
        perror("Ошибка создания потока читателя");
        exit(EXIT_FAILURE);
    }

    pthread_join(writer, NULL);
    pthread_join(reader, NULL);

    printf("Работа завершена.\n");
    return 0;
}