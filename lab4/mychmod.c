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

// теперь функция принимает файлы и обрабатывает + / -
void apply_symbolic_mode(const char *mode_str, int num_files, char **files) {
    // чек, что строка начинается с '+' или '-'
    if (mode_str[0] != '+' && mode_str[0] != '-') {
        fprintf(stderr, "ошибка: неподдерживаемый оператор в '%s'\n", mode_str);
        exit(EXIT_FAILURE);
    }

    char op = mode_str[0];
    const char *perms = mode_str + 1;

    if (*perms == '\0') {
        fprintf(stderr, "ошибка: не указаны права в '%s'\n", mode_str);
        exit(EXIT_FAILURE);
    }

    // собир бит маску для всех катгорий
    mode_t mask = 0;
    for (int i = 0; perms[i] != '\0'; i++) {
        switch (perms[i]) {
            case 'r':
                mask |= S_IRUSR | S_IRGRP | S_IROTH;
                break;
            case 'w':
                mask |= S_IWUSR | S_IWGRP | S_IWOTH;
                break;
            case 'x':
                mask |= S_IXUSR | S_IXGRP | S_IXOTH;
                break;
            default:
                fprintf(stderr, "ошибка: неизвестное право '%c' в '%s'\n", perms[i], mode_str);
                exit(EXIT_FAILURE);
        }
    }

    // прим измен к каждому файлу
    for (int i = 0; i < num_files; i++) {
        struct stat sb;
        if (stat(files[i], &sb) == -1) {
            perror(files[i]);
            continue;
        }

        mode_t current_mode = sb.st_mode;
        mode_t new_mode;

        if (op == '+') {
            new_mode = current_mode | mask;   // устн бит
        } else { // op == '-'
            new_mode = current_mode & ~mask;  // сброс бит
        }

        if (chmod(files[i], new_mode) == -1) {
            perror(files[i]);
        }
    }
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
        apply_symbolic_mode(mode_str, num_files, files);
    }

    return 0;
}
