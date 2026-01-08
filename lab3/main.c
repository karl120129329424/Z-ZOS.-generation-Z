#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
/*
handle_sigint, процесс прерван по сигналу SIGINT - код 1
handle_sigterm, процесс завершён по сигналу SIGTERM - код 2
дочерний процесс завершился успешно по логике - код 3
*/

// выз при норм завершении через atexit
void exit_handler(void) {
    printf("Процесс PID=%d завершается (atexit)\n", (int)getpid());
}

// обраб-к SIGINT через signal()
void handle_sigint(int sig) {
    printf("Процесс PID=%d получил сигнал: %s (обработан через signal)\n",
           (int)getpid(), strsignal(sig));
    exit(1);
}

// обраб-к SIGTERM через sigaction()
void handle_sigterm(int sig) {
    printf("Процесс PID=%d получил сигнал: %s (обработан через sigaction)\n",
           (int)getpid(), strsignal(sig));
    exit(2);
}

int main(void) {
    // 1
    if (atexit(exit_handler) != 0) {
        perror("atexit");
        return EXIT_FAILURE;
    }

    // 2 SIGINT через signal()
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        perror("signal SIGINT");
        return EXIT_FAILURE;
    }

    // 3 SIGTERM через sigaction
    struct sigaction sa;
    sa.sa_handler = handle_sigterm;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction SIGTERM");
        return EXIT_FAILURE;
    }

    // 4 вывод PID и PPID род проц
    printf("Родительский процесс: PID=%d, PPID=%d\n", (int)getpid(), (int)getppid());

    // 5 созд доч процесс
    pid_t child_pid = fork();

    if (child_pid == -1) {
        // ошибка fork
        perror("fork");
        return EXIT_FAILURE;
    }

    if (child_pid == 0) {
        // доч проц
        printf("Доч процесс: PID=%d, PPID=%d\n", (int)getpid(), (int)getppid());
        sleep(2); // имитация работы
        printf("Доч процесс завершается с кодом 3\n");
        return 3;
    } else {
        // род проц
        int status;
        printf("Род ждёт завершения доч процесса...\n");

        // 6 ожид заверш доч
        if (waitpid(child_pid, &status, 0) == -1) {
            perror("waitpid");
            return EXIT_FAILURE;
        }

        // 7 Анализ заверш доч проц
        if (WIFEXITED(status)) {
            printf("Дочерний процесс завершился нормально. Код: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Дочерний процесс завершился из-за сигнала: %s\n",
                   strsignal(WTERMSIG(status)));
        }
    }

    return 0;
}