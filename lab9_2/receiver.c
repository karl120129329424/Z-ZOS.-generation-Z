#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <time.h>

#define SHM_KEY 0x1234
#define SEM_KEY 0x5678
#define BUF_SIZE 256

// Вспомогательная структура для semctl
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

int main() {
    int shm_id = shmget(SHM_KEY, BUF_SIZE, 0666);
    if (shm_id == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    char *shared_buf = (char *)shmat(shm_id, NULL, 0);
    if (shared_buf == (char *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    int sem_id = semget(SEM_KEY, 1, 0666);
    if (sem_id == -1) {
        perror("semget failed");
        exit(EXIT_FAILURE);
    }

    struct sembuf sem_lock = {0, -1, 0};
    struct sembuf sem_unlock = {0, 1, 0};

    printf("[Receiver] PID: %d\n", getpid());
    printf("[Receiver] Готов к приёму...\n");

    while (1) {
        if (semop(sem_id, &sem_lock, 1) == -1) {
            perror("semop lock failed");
            break;
        }

        char received[BUF_SIZE];
        strncpy(received, shared_buf, BUF_SIZE);

        if (semop(sem_id, &sem_unlock, 1) == -1) {
            perror("semop unlock failed");
            break;
        }

        // Выводим данные
        time_t now = time(NULL);
        char time_str[64];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

        printf("[Receiver] Время: %s | Мой PID: %d | Получено: %s\n",
               time_str, getpid(), received);

        sleep(1);
    }

    shmdt(shared_buf);
    return 0;
}