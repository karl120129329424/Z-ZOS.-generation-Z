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

    mkfifo(FIFO_NAME, 0666);

    parent_time = time(NULL);
    snprintf(buffer, sizeof(buffer),
             "Parent PID: %d, time: %s",
             getpid(), ctime(&parent_time));

    fd = open(FIFO_NAME, O_WRONLY);
    if (fd == -1) {
        perror("open fifo for write");
        exit(EXIT_FAILURE);
    }

    printf("Parent started. Waiting 10 seconds before sending...\n");
    sleep(10);

    write(fd, buffer, strlen(buffer) + 1);
    close(fd);

    printf("Parent finished sending.\n");
    return 0;
}