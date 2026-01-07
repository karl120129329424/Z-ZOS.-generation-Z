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
        //-
        
        //--
        file_count++;
        fprintf(stderr, "[DEBUG] добавлено: %s (всего: %d)\n", entry->d_name, file_count);
        //--
    }

    closedir(dir);

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