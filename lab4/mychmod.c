#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int is_octal_string(const char *s) {
    if (!s || !*s) return 0;
    for (int i = 0; s[i] != '\0'; i++) {
        if (s[i] < '0' || s[i] > '7') {
            return 0;
        }
    }
    return 1;
}

void apply_octal_mode(const char *mode_str, int num_files, char **files) {
    mode_t new_mode = (mode_t)strtol(mode_str, NULL, 8);
    for (int i = 0; i < num_files; i++) {
        if (chmod(files[i], new_mode) == -1) {
            perror(files[i]);
        }
    }
}

void apply_symbolic_mode(const char *mode_str) {
    fprintf(stderr, "ошибка: симв режим ('%s') не подд\n", mode_str);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "использование: %s <режим> <файл>...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *mode_str = argv[1];
    int num_files = argc - 2;
    char **files = argv + 2;

    if (is_octal_string(mode_str)) {
        apply_octal_mode(mode_str, num_files, files);
    } else {
        apply_symbolic_mode(mode_str);
    }

    return 0;
}
