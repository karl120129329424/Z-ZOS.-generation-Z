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

    // Создаём pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Создаём дочерний процесс
    child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }


    if (child_pid == 0) {
        // === Дочерний процесс ===
        close(pipefd[1]); // Закрываем запись

        // Читаем данные от родителя
        ssize_t bytes = read(pipefd[0], buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0'; // Обнуляем конец строки
        }

        child_time = time(NULL);
        printf("Дочерний процесс (PID: %d):\n", getpid());
        printf("Текущее время: %s", ctime(&child_time));
        printf("Полученная строка: %s\n", buffer);

        close(pipefd[0]);
        exit(EXIT_SUCCESS);

    } else {
        // === Родительский процесс ===
        close(pipefd[0]); // Закрываем чтение

        parent_time = time(NULL);
        snprintf(buffer, sizeof(buffer),
                 "Родительский PID: %d, время: %s",
                 getpid(), ctime(&parent_time));

        // Ждём минимум 5 секунд — требование задания
        sleep(5);

        // Отправляем данные в дочерний процесс
        write(pipefd[1], buffer, strlen(buffer) + 1);

        close(pipefd[1]);

        // Ждём завершения потомка
        wait(NULL);

        printf("Родительский процесс завершил работу.\n");
    }

    return 0;
}
