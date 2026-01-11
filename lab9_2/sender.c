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
// На macOS она уже определена, поэтому объявляем только при необходимости
#ifndef __APPLE__
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};
#endif

int main() {
    
    int shm_id = shmget(SHM_KEY, BUF_SIZE, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    char *shared_buf = (char *)shmat(shm_id, NULL, 0);
    if (shared_buf == (char *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    int sem_id = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("semget failed");
        exit(EXIT_FAILURE);
    }

    // На macOS semctl(..., SETVAL, ...) смонительно работает.
    // Вместо этого полагаемся на то, что семафор изначально равен 0,
    // и первый вызов semop с +1 (V) установит его в 1.
    // Но так как sender должен писать первым, мы делаем "первый V" вручную
    // перед циклом, чтобы разблокировать возможное ожидание.

    struct sembuf sem_unlock_first = {0, 1, 0};  // V — разрешить первый доступ
    if (semop(sem_id, &sem_unlock_first, 1) == -1) {
        perror("semop initial unlock failed");
        exit(EXIT_FAILURE);
    }

    struct sembuf sem_lock = {0, -1, 0};   // P
    struct sembuf sem_unlock = {0, 1, 0};  // V

    printf("[Sender] PID: %d\n", getpid());
    printf("[Sender] Готов к передаче...\n");

    while (1) {
        time_t now = time(NULL);
        char time_str[64];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

        char msg[BUF_SIZE];
        snprintf(msg, BUF_SIZE, "Time: %s | PID: %d", time_str, getpid());

        if (semop(sem_id, &sem_lock, 1) == -1) {
            perror("semop lock failed");
            break;
        }

        strncpy(shared_buf, msg, BUF_SIZE);

        if (semop(sem_id, &sem_unlock, 1) == -1) {
            perror("semop unlock failed");
            break;
        }

        printf("[Sender] Отправлено: %s\n", msg);
        sleep(3);
    }

    // Очистка (не достигнем, но для порядка)
    shmdt(shared_buf);
    return 0;
}