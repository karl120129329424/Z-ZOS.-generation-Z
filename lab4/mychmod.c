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

void apply_symbolic_mode(const char *mode_str, int num_files, char **files) {
    const char *p = mode_str;
    
    // определяем категории
    int who_mask = 0; // битовая маска: 1=u, 2=g, 4=o
    while (*p == 'u' || *p == 'g' || *p == 'o' || *p == 'a') {
        switch (*p) {
            case 'u': who_mask |= 1; break;
            case 'g': who_mask |= 2; break;
            case 'o': who_mask |= 4; break;
            case 'a': who_mask |= 7; break; // a = u+g+o
        }
        p++;
    }

    //  по умолчанию 'a'
    if (who_mask == 0) {
        who_mask = 7; // все категории
    }

    // провер оператор
    if (*p != '+' && *p != '-' && *p != '=') {
        fprintf(stderr, "ошибка: ожидается +, - или = в '%s'\n", mode_str);
        exit(EXIT_FAILURE);
    }
    char op = *p++;
    
    // провер права
    if (*p == '\0' && op != '=') {
    fprintf(stderr, "ошибка: не указаны права в '%s'\n", mode_str);
    exit(EXIT_FAILURE);
    }

    // собираем маску прав для каждой категории
    mode_t user_mask = 0, group_mask = 0, other_mask = 0;
    for (int i = 0; p[i] != '\0'; i++) {
        switch (p[i]) {
            case 'r':
                if (who_mask & 1) user_mask |= S_IRUSR;
                if (who_mask & 2) group_mask |= S_IRGRP;
                if (who_mask & 4) other_mask |= S_IROTH;
                break;
            case 'w':
                if (who_mask & 1) user_mask |= S_IWUSR;
                if (who_mask & 2) group_mask |= S_IWGRP;
                if (who_mask & 4) other_mask |= S_IWOTH;
                break;
            case 'x':
                if (who_mask & 1) user_mask |= S_IXUSR;
                if (who_mask & 2) group_mask |= S_IXGRP;
                if (who_mask & 4) other_mask |= S_IXOTH;
                break;
            default:
                fprintf(stderr, "ошибка: неизвестное право '%c' в '%s'\n", p[i], mode_str);
                exit(EXIT_FAILURE);
        }
    }

    mode_t full_mask = user_mask | group_mask | other_mask;

    // применяем к каждому файлу
    for (int i = 0; i < num_files; i++) {
        struct stat sb;
        if (stat(files[i], &sb) == -1) {
            perror(files[i]);
            continue;
        }

        mode_t current = sb.st_mode;
        mode_t new_mode = current;

        if (op == '+') {
            new_mode |= full_mask;
        } else if (op == '-') {
            new_mode &= ~full_mask;
        } else if (op == '=') {
            // дополнено(выполнено)
            mode_t clear_mask = 0;
            if (who_mask & 1) clear_mask |= S_IRWXU; // все права владельца
            if (who_mask & 2) clear_mask |= S_IRWXG; // все права группы
            if (who_mask & 4) clear_mask |= S_IRWXO; // все остальные права

            new_mode = current & ~clear_mask;
            new_mode |= full_mask;
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
