#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#define SHM_KEY 1234
#define SHM_SIZE 1024
#define PID_FILE "/tmp/sender.pid"

int main() {
    // Пытаемся создать PID-файл с эксклюзивным доступом
    int fd = open(PID_FILE, O_CREAT | O_RDWR | O_EXCL, 0644);
    if (fd == -1) {
        if (errno == EEXIST) {
            // Файл уже существует — возможно, sender уже запущен
            FILE *f = fopen(PID_FILE, "r");
            if (f) {
                int old_pid;
                if (fscanf(f, "%d", &old_pid) == 1) {
                    if (kill(old_pid, 0) == 0) {
                        printf("Ошибка: sender уже запущен (PID: %d)\n", old_pid);
                        fclose(f);
                        exit(1);
                    }
                }
                fclose(f);
            }
            // Если процесс мёртв — удалим старый файл и попробуем снова
            unlink(PID_FILE);
            fd = open(PID_FILE, O_CREAT | O_RDWR | O_EXCL, 0644);
            if (fd == -1) {
                perror("Не удалось создать PID-файл даже после удаления");
                exit(1);
            }
        } else {
            perror("Не удалось создать PID-файл");
            exit(1);
        }
    }

    // Записываем свой PID
    char pid_str[32];
    snprintf(pid_str, sizeof(pid_str), "%d\n", getpid());
    write(fd, pid_str, strlen(pid_str));
    close(fd); // дескриптор можно закрыть — файл остаётся

    // Создание разделяемой памяти
    int shm_id = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        unlink(PID_FILE);
        exit(1);
    }

    char *shm_ptr = (char *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (char *)-1) {
        perror("shmat");
        shmctl(shm_id, IPC_RMID, NULL);
        unlink(PID_FILE);
        exit(1);
    }

    printf("Sender запущен. PID: %d\n", getpid());

    while (1) {
        time_t now = time(NULL);
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "Time: %ld, PID: %d", now, getpid());
        strncpy(shm_ptr, buffer, SHM_SIZE - 1);
        shm_ptr[SHM_SIZE - 1] = '\0';
        printf("Отправлено: %s\n", buffer);
        sleep(1);
    }

    // Очистка (недостижима, но на случай будущих изменений)
    shmdt(shm_ptr);
    shmctl(shm_id, IPC_RMID, NULL);
    unlink(PID_FILE);
    return 0;
}