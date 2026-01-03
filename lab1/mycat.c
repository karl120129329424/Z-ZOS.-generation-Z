#include <stdio.h>
#include <string.h>

void cat_stream(FILE *stream, int number_all, int number_nonblank, int *line_counter) {
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), stream) != NULL) {
        int should_number = 0;
        if (number_nonblank) {
            // Пустая строка: начинается с \n или \r (Windows)
            if (buffer[0] != '\n' && buffer[0] != '\r') {
                should_number = 1;
            }
        } else if (number_all) {
            should_number = 1;
        }

        if (should_number) {
            printf("%6d\t", (*line_counter)++);
        }
        fputs(buffer, stdout);
    }
}

int main(int argc, char *argv[]) {
    int number_all = 0;        // флаг -n
    int number_nonblank = 0;   // флаг -b
    int file_start = 1;

    // Парсим флаги в начале аргументов
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0) {
            number_all = 1;
            file_start++;
        } else if (strcmp(argv[i], "-b") == 0) {
            number_nonblank = 1;
            file_start++;
        } else {
            break; // останавливаемся на первом не-флаге
        }
    }

    int line_num = 1;

    if (argc == file_start) {
        // Только stdin
        cat_stream(stdin, number_all, number_nonblank, &line_num);
    } else {
        for (int i = file_start; i < argc; i++) {
            if (strcmp(argv[i], "-") == 0) {
                cat_stream(stdin, number_all, number_nonblank, &line_num);
            } else {
                FILE *fp = fopen(argv[i], "r");
                if (fp == NULL) {
                    perror(argv[i]);
                } else {
                    cat_stream(fp, number_all, number_nonblank, &line_num);
                    fclose(fp);
                }
            }
        }
    }
    return 0;
}