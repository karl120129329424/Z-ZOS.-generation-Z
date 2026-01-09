#include <stdio.h>
#include <string.h>

void print_help() {
    printf("Usage:\n");
    printf("  ./archiver <archive_name> -i(--input) <file>\n");
    printf("  ./archiver <archive_name> -e(--extract) <file>\n");
    printf("  ./archiver <archive_name> -s(--stat)\n");
    printf("  ./archiver -h(--help)\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help();
        return 1;
    }

    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        print_help();
        return 0;
    }

    // Пока не поддерживаем другие команды
    fprintf(stderr, "Error: unknown command\n");
    print_help();
    return 1;
}