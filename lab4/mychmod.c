#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// проверка, что строка сост из 0–7
int is_octal_string(const char *s) {
    if (!s || !*s) return 0;
    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] < '0' || s[i] > '7') {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "использование: %s <режим> <файл>...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *mode_str = argv[1];

    // ТОЛЬКО октальный режим (сейчас)
    if (!is_octal_string(mode_str)) {
        fprintf(stderr, "ошибка: только окт_реж\n");
        exit(EXIT_FAILURE);
    }

    // превр стр в число по основанию 8
    mode_t new_mode = (mode_t)strtol(mode_str, NULL, 8);

    // новые права ко всем файлам
    for (int i = 2; i < argc; i++) {
        if (chmod(argv[i], new_mode) == -1) {
            perror(argv[i]);
        }
    }

    return 0;
}
