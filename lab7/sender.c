// sender.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024

int main() {
    // Создание разделяемой памяти
    int shm_id = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }

    // Подключение к разделяемой памяти
    char *shm_ptr = (char *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (char *)-1) {
        perror("shmat");
        exit(1);
    }

    printf("Sender запущен. PID: %d\n", getpid());

    // Бесконечный цикл отправки данных
    while (1) {
        time_t now = time(NULL);
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "Time: %ld, PID: %d", now, getpid());
        strncpy(shm_ptr, buffer, SHM_SIZE - 1);
        shm_ptr[SHM_SIZE - 1] = '\0';  // гарантируем завершение строки
        printf("Отправлено: %s\n", buffer);
        sleep(1);
    }

    // Очистка
    shmdt(shm_ptr);
    return 0;
}