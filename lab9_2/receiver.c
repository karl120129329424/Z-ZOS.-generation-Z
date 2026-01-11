#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>

#define SHM_KEY 0x1234
#define BUF_SIZE 256

int main() {
    int shm_id = shmget(SHM_KEY, BUF_SIZE, 0666);
    if (shm_id == -1) {
        perror("shmget failed (segment not found)");
        exit(EXIT_FAILURE);
    }

    char *shared_buf = (char *)shmat(shm_id, NULL, 0);
    if (shared_buf == (char *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    printf("[Receiver] PID: %d\n", getpid());
    printf("[Receiver] Подключён к разделяемой памяти. Начинаю приём...\n");

    while (1) {
        char received[BUF_SIZE];
        strncpy(received, shared_buf, BUF_SIZE);

        time_t now = time(NULL);
        char time_str[64];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

        printf("[Receiver] Время: %s | Мой PID: %d | Получено: %s\n",
               time_str, getpid(), received);

        sleep(1); // читаем каждую секунду
    }

    return 0;
}