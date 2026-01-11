#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

char buffer[100];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int write_counter = 0;

void* writer_thread(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        write_counter++;
        // запись строки вида "Запись N"
        snprintf(buffer, sizeof(buffer), "Запись %d", write_counter);

        printf("Пишущий поток записал: %s\n", buffer);

        pthread_mutex_unlock(&mutex);

        sleep(1); // пауза, чтобы не перегружать вывод
    }
    return NULL;
}