#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
//макось, мммммм


int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    int flag_a = 0;
    int flag_l = 0;

    int opt;
    while ((opt = getopt(argc, argv, "la")) != -1) {
        switch (opt) {
            case 'a':
                flag_a = 1;
                break;
            case 'l':
                flag_l = 1;
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

    // сбор имён в массив
    char **file_list = NULL;   // массив строк (указателей на char)
    int file_count = 0;        // кол-во скок добавили
    int alloc_size = 0;        // скок памяти выделено (в элементах)
    //-

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (!flag_a && entry->d_name[0] == '.') {
            continue;
        }

        // нужно ли расширить массив?
        if (file_count >= alloc_size) {
            alloc_size = alloc_size ? alloc_size * 2 : 8;
            file_list = realloc(file_list, alloc_size * sizeof(char *));
            if (!file_list) {
                perror("realloc");
                exit(1);
            }
        }

        file_list[file_count] = strdup(entry->d_name); // копируем имя (strdup = malloc + strcpy)
        if (!file_list[file_count]) {
            perror("strdup");
            exit(1);
        }
        file_count++;
    }

    closedir(dir);


    // Сортируем (как раньше)
    for (int i = 0; i < file_count - 1; i++) {
        for (int j = i + 1; j < file_count; j++) {
            if (strcmp(file_list[i], file_list[j]) > 0) {
                char *tmp = file_list[i];
                file_list[i] = file_list[j];
                file_list[j] = tmp;
            }
        }
    }

    // Выводим
    for (int i = 0; i < file_count; i++) {
        if (flag_l) {
            // Для -l: пока просто размер
            char full_path[PATH_MAX];
            snprintf(full_path, sizeof(full_path), "%s/%s", path, file_list[i]);

            struct stat st;
            if (lstat(full_path, &st) == 0) {
                printf("%lu %s\n", (unsigned long)st.st_size, file_list[i]);
            } else {
                perror(full_path);
            }
        } else {
            printf("%s\n", file_list[i]);
        }
    }

    // вывод из массива
    for (int i = 0; i < file_count; i++) {
        printf("%s\n", file_list[i]);
    }

    // Очистка памятик
    for (int i = 0; i < file_count; i++) {
        free(file_list[i]);
    }
    free(file_list);
    //-

    return 0;
}