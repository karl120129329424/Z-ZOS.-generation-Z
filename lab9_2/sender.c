#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>

#define SHM_KEY 0x1234
#define BUF_SIZE 256

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

    printf("[Sender] PID: %d\n", getpid());
    printf("[Sender] Разделяемая память подключена. Начинаю передачу...\n");

    while (1) {
        time_t now = time(NULL);
        char time_str[64];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

        char msg[BUF_SIZE];
        snprintf(msg, BUF_SIZE, "Time: %s | PID: %d", time_str, getpid());

        strncpy(shared_buf, msg, BUF_SIZE);

        printf("[Sender] Отправлено: %s\n", msg);
        sleep(3); // каждые 3 секунды
    }

    return 0;
}