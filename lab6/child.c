#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#define FIFO_NAME "myfifo"

int main(void) {
    int fd;
    char buffer[256];
    time_t child_time;

    // Открываем FIFO для чтения
    fd = open(FIFO_NAME, O_RDONLY);
    if (fd == -1) {
        perror("open fifo for read");
        exit(EXIT_FAILURE);
    }

    // Читаем строку
    ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);
    if (bytes > 0) {
        buffer[bytes] = '\0';
    }
    close(fd);

    // Получаем время дочернего процесса
    child_time = time(NULL);
    printf("Дочерний процесс (PID: %d):\n", getpid());
    printf("Текущее время: %s", ctime(&child_time));
    printf("Полученная строка: %s\n", buffer);

    // Удаляем FIFO после использования
    unlink(FIFO_NAME);

    return 0;
}
