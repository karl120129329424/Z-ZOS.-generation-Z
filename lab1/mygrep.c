#include <stdio.h>
#include <string.h>
#include <ctype.h>

int contains_ignore_case(const char *haystack, const char *needle) {
    if (!haystack || !needle) return 0;
    size_t hlen = strlen(haystack);
    size_t nlen = strlen(needle);
    if (nlen == 0) return 1;

    for (size_t i = 0; i + nlen <= hlen; i++) {
        int match = 1;
        for (size_t j = 0; j < nlen; j++) {
            if (tolower((unsigned char)haystack[i + j]) !=
                tolower((unsigned char)needle[j])) {
                match = 0;
                break;
            }
        }
        if (match) return 1;
    }
    return 0;
}

void grep_stream(FILE *stream, const char *pattern, int ignore_case) {
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), stream) != NULL) {
        int found = 0;
        if (ignore_case) {
            found = contains_ignore_case(buffer, pattern);
        } else {
            found = (strstr(buffer, pattern) != NULL);
        }
        if (found) {
            fputs(buffer, stdout);
        }
    }
}

int main(int argc, char *argv[]) {
    int ignore_case = 0;
    int pattern_index = 1;

    if (argc >= 2 && strcmp(argv[1], "-i") == 0) {
        ignore_case = 1;
        pattern_index = 2;
    }

    if (argc < pattern_index + 1) {
        fprintf(stderr, "Usage: %s [-i] pattern [file...]\n", argv[0]);
        return 1;
    }

    const char *pattern = argv[pattern_index];
    int file_start = pattern_index + 1;

    if (argc == file_start) {
        grep_stream(stdin, pattern, ignore_case);
    } else {
        for (int i = file_start; i < argc; i++) {
            FILE *fp = fopen(argv[i], "r");
            if (fp == NULL) {
                perror(argv[i]);
            } else {
                grep_stream(fp, pattern, ignore_case);
                fclose(fp);
            }
        }
    }
    return 0;
}