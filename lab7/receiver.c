// receiver.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024

int main() {
    // Подключение к существующей разделяемой памяти
    int shm_id = shmget(SHM_KEY, SHM_SIZE, 0666);
    if (shm_id == -1) {
        perror("shmget: разделяемая память не найдена. Запустите sender.");
        exit(1);
    }

    char *shm_ptr = (char *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (char *)-1) {
        perror("shmat");
        exit(1);
    }

    printf("Receiver запущен. PID: %d\n", getpid());

    // Бесконечный цикл чтения данных
    while (1) {
        time_t now = time(NULL);
        char received[SHM_SIZE];
        strncpy(received, shm_ptr, SHM_SIZE - 1);
        received[SHM_SIZE - 1] = '\0';

        if (strlen(received) > 0) {
            printf("Текущее время: %ld, Мой PID: %d, Принято: %s\n", now, getpid(), received);
        } else {
            printf("Нет данных от sender.\n");
        }

        sleep(1);
    }

    // Очистка (недостижима)
    shmdt(shm_ptr);
    return 0;
}