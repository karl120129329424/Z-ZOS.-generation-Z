#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

int main(void) {
    int pipefd[2];
    pid_t child_pid;
    char buffer[256];
    time_t parent_time, child_time;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        // Child process
        close(pipefd[1]);
        ssize_t bytes = read(pipefd[0], buffer, sizeof(buffer) - 1);
        if (bytes > 0) buffer[bytes] = '\0';
        child_time = time(NULL);
        printf("Child process (PID: %d):\n", getpid());
        printf("Current time: %s", ctime(&child_time));
        printf("Received string: %s\n", buffer);
        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    } else {
        // Parent process
        close(pipefd[0]);
        parent_time = time(NULL);
        snprintf(buffer, sizeof(buffer),
                 "Parent PID: %d, time: %s",
                 getpid(), ctime(&parent_time));
        sleep(5);
        write(pipefd[1], buffer, strlen(buffer) + 1);
        close(pipefd[1]);
        wait(NULL);
        printf("Parent process finished.\n");
    }

    return 0;
}
