#include <stdio.h>
#include <string.h>

void grep_stream(FILE *stream, const char *pattern) {
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), stream) != NULL) {
        if (strstr(buffer, pattern) != NULL) {
            fputs(buffer, stdout);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s pattern [file...]\n", argv[0]);
        return 1;
    }

    const char *pattern = argv[1];
    int file_start = 2;

    if (argc == file_start) {
        // Только stdin
        grep_stream(stdin, pattern);
    } else {
        for (int i = file_start; i < argc; i++) {
            FILE *fp = fopen(argv[i], "r");
            if (fp == NULL) {
                perror(argv[i]);
            } else {
                grep_stream(fp, pattern);
                fclose(fp);
            }
        }
    }
    return 0;
}