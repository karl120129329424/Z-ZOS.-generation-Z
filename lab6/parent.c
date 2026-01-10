#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

#define FIFO_NAME "myfifo"

int main(void) {
    int fd;
    char buffer[256];
    time_t parent_time;

    // Создаём FIFO (если не существует)
    mkfifo(FIFO_NAME, 0666);

    // Получаем текущее время
    parent_time = time(NULL);
    snprintf(buffer, sizeof(buffer),
             "Родительский PID: %d, время: %s",
             getpid(), ctime(&parent_time));

    // Открываем FIFO для записи
    fd = open(FIFO_NAME, O_WRONLY);
    if (fd == -1) {
        perror("open fifo for write");
        exit(EXIT_FAILURE);
    }

    printf("Родитель запущен. Жду 10 секунд перед отправкой...\n");
    sleep(10);

    // Отправляем данные
    write(fd, buffer, strlen(buffer) + 1);
    close(fd);

    printf("Родитель завершил отправку.\n");
    return 0;
}
