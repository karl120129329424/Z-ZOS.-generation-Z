#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    int flag_a = 0;

    int opt;
    while ((opt = getopt(argc, argv, "a")) != -1) {
        switch (opt) {
            case 'a':
                flag_a = 1;
                break;
            default:
                fprintf(stderr, "Использование: %s [-a] [папка]\n", argv[0]);
                return 1;
        }
    }

    const char *path = ".";
    if (optind < argc) {
        path = argv[optind];
    }

    // Откр директорию
    DIR *dir = opendir(path);
    if (!dir) {
        perror(path);
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Пропускаем скрытые, если флаг -a НЕ задан
        if (!flag_a && entry->d_name[0] == '.') {
            continue;
        }
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
    return 0;
}