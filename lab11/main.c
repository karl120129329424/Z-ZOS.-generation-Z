#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_READERS 10
#define ARRAY_SIZE 5

char shared_array[ARRAY_SIZE];
int write_counter = 0;

int active_readers = 0;   // сколько читателей сейчас внутри критической секции
int writer_active = 0;    // 1, если пишущий выполняет запись
int writer_waiting = 0;   // 1, если пишущий ждёт — чтобы избежать его голодания

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


void* writer(void* arg) {
    (void)arg;

    while (1) {
        pthread_mutex_lock(&mutex);

        writer_waiting = 1;
        while (active_readers > 0 || writer_active) {
            pthread_cond_wait(&cond, &mutex);
        }
        writer_waiting = 0;

        writer_active = 1;
        pthread_mutex_unlock(&mutex);

        // --- Критическая секция записи (вне мьютекса, но защищена флагами) ---
        write_counter++;
        for (int i = 0; i < ARRAY_SIZE; i++) {
            shared_array[i] = (char)(write_counter % 128); // чтобы оставаться в допустимом диапазоне char
        }

        printf("Writer (tid=%p): wrote %d to array\n", (void*)pthread_self(), write_counter);

        // --- Конец критической секции записи ---

        pthread_mutex_lock(&mutex);
        writer_active = 0;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);

        sleep(1);
    }
    return NULL;
}

void* reader(void* arg) {
    (void)arg;
    long tid = (long)arg;

    while (1) {
        pthread_mutex_lock(&mutex);
        while (writer_active || writer_waiting) {
            pthread_cond_wait(&cond, &mutex);
        }

        active_readers++;
        pthread_mutex_unlock(&mutex);

        printf("Reader (tid=%ld): [", tid);
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("%d", (int)(unsigned char)shared_array[i]);
            if (i < ARRAY_SIZE - 1) printf(", ");
        }
        printf("]\n");

        pthread_mutex_lock(&mutex);
        active_readers--;

        if (active_readers == 0) {
            pthread_cond_signal(&cond);
        }

        pthread_mutex_unlock(&mutex);

        usleep(500000); // 0.5 сек
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

    printf("Лаба запущена. Ctrl+C для остановки.\n");

    pthread_join(writer_thread, NULL);
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(reader_threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    return 0;
}