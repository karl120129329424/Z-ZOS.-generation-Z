#include <stdio.h>
#include <string.h>

void cat_stream(FILE *stream, int number_all, int number_nonblank, int show_ends, int *line_counter) {
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), stream) != NULL) {
        int should_number = 0;
        if (number_nonblank) {
            if (buffer[0] != '\n' && buffer[0] != '\r') {
                should_number = 1;
            }
        } else if (number_all) {
            should_number = 1;
        }

        if (should_number) {
            printf("%6d\t", (*line_counter)++);
        }

        if (show_ends) {
            char out_buffer[1026]; // 1024 + '$' + '\n' + '\0'
            size_t len = strlen(buffer);
            size_t copy_len = len;

            // Убираем \n и \r\n с конца
            if (len > 0 && buffer[len-1] == '\n') {
                copy_len = len - 1;
                if (copy_len > 0 && buffer[copy_len-1] == '\r') {
                    copy_len--;
                }
            }

            // Копируем "тело" строки
            memcpy(out_buffer, buffer, copy_len);
            out_buffer[copy_len] = '$';
            out_buffer[copy_len + 1] = '\n';
            out_buffer[copy_len + 2] = '\0';

            fputs(out_buffer, stdout);
        } else {
            fputs(buffer, stdout);
        }
    }
}

int main(int argc, char *argv[]) {
    int number_all = 0;
    int number_nonblank = 0;
    int show_ends = 0;
    int file_start = 1;

    // Парсим флаги в начале
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0) {
            number_all = 1;
            file_start++;
        } else if (strcmp(argv[i], "-b") == 0) {
            number_nonblank = 1;
            file_start++;
        } else if (strcmp(argv[i], "-E") == 0) {
            show_ends = 1;
            file_start++;
        } else {
            break;
        }
    }

    int line_num = 1;

    if (argc == file_start) {
        cat_stream(stdin, number_all, number_nonblank, show_ends, &line_num);
    } else {
        for (int i = file_start; i < argc; i++) {
            if (strcmp(argv[i], "-") == 0) {
                cat_stream(stdin, number_all, number_nonblank, show_ends, &line_num);
            } else {
                FILE *fp = fopen(argv[i], "r");
                if (fp == NULL) {
                    perror(argv[i]);
                } else {
                    cat_stream(fp, number_all, number_nonblank, show_ends, &line_num);
                    fclose(fp);
                }
            }
        }
    }
    return 0;
}