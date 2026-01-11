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

// Функция для читающего потока
void* reader_thread(void* arg) {
    long tid = (long)arg; // получаем идентификатор потока

    while (1) {
        pthread_mutex_lock(&mutex);

        printf("Читающий поток %ld: текущее состояние буфера — '%s'\n", tid, buffer);

        pthread_mutex_unlock(&mutex);
        sleep(2); // читаем реже, чем пишем
    }
    return NULL;
}

int main() {
    pthread_t writer_tid;
    pthread_t reader_tids[10];

    // пишущий поток
    if (pthread_create(&writer_tid, NULL, writer_thread, NULL) != 0) {
        perror("Не удалось создать пишущий поток");
        exit(EXIT_FAILURE);
    }

    // 10 читающих потоков
    for (long i = 0; i < 10; i++) {
        if (pthread_create(&reader_tids[i], NULL, reader_thread, (void*)i) != 0) {
            perror("Не удалось создать читающий поток");
            exit(EXIT_FAILURE);
        }
    }

    sleep(30);

    printf("Программа завершена.\n");
    return 0;
}