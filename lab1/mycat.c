#include <stdio.h>
#include <string.h>

void cat_stream(FILE *stream) {
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), stream) != NULL) {
        fputs(buffer, stdout);
    }
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        // Нет аргументов — читаем stdin
        cat_stream(stdin);
    } else {
        // Есть аргументы — обрабатываем каждый
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-") == 0) {
                // '-' означает stdin (соглашение Unix)
                cat_stream(stdin);
            } else {
                // Пытаемся открыть файл
                FILE *fp = fopen(argv[i], "r");
                if (fp == NULL) {
                    // Ошибка: печатаем как perror
                    perror(argv[i]);
                } else {
                    // Успешно: читаем и закрываем
                    cat_stream(fp);
                    fclose(fp);
                }
            }
        }
    }
    return 0;
}