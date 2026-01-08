#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "использование: %s [режим] [файл]...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("Режим: %s\n", argv[1]);
    printf("Будем применять к %d файл-ам:\n", argc - 2);
    for (int i = 2; i < argc; i++) {
        printf("  - %s\n", argv[i]);
    }

    return 0;
}


