#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define ARRAY_SIZE 10
char shared_array[ARRAY_SIZE] = "init";

void* writer_thread(void* arg) {
    int counter = 0;
    while (counter < 5) {
        snprintf(shared_array, ARRAY_SIZE, "%d", counter);
        printf("Писатель записал: %s\n", shared_array);
        usleep(1000000); // 1 секунда
        counter++;
    }
    return NULL;
}

void* reader_thread(void* arg) {
    pthread_t tid = pthread_self();
    int counter = 0;
    while (counter < 5) {
        printf("Читатель [TID: %lu] прочитал: %s\n", tid, shared_array);
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