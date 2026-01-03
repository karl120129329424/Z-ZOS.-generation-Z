#include <stdio.h>
#include <string.h>

void cat_stream(FILE *stream, int number_lines, int *line_counter) {
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), stream) != NULL) {
        if (number_lines) {
            printf("%6d\t", (*line_counter)++);
        }
        fputs(buffer, stdout);
    }
}

int main(int argc, char *argv[]) {
    int show_line_numbers = 0;
    int file_start = 1;

    // Парсим флаги: ищем "-n"
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0) {
            show_line_numbers = 1;
            file_start++;  // пропускаем этот аргумент
        } else {
            break;  // флаги идут в начале — как в cat
        }
    }

    int line_num = 1;

    if (argc == file_start) {
        // Только stdin
        cat_stream(stdin, show_line_numbers, &line_num);
    } else {
        for (int i = file_start; i < argc; i++) {
            if (strcmp(argv[i], "-") == 0) {
                cat_stream(stdin, show_line_numbers, &line_num);
            } else {
                FILE *fp = fopen(argv[i], "r");
                if (fp == NULL) {
                    perror(argv[i]);
                } else {
                    cat_stream(fp, show_line_numbers, &line_num);
                    fclose(fp);
                }
            }
        }
    }
    return 0;
}