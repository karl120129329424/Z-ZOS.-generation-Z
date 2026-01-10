#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define HEADER_MAGIC "ARCHV1"

typedef struct {
    char magic[8];       // "ARCHV1\0\0"
    char filename[16];   // до 15 символов + '\0'
    off_t original_size;
    mode_t mode;
    uid_t uid;
    gid_t gid;
    time_t mtime;
} FileHeader;

void print_help() {
    printf("Usage:\n");
    printf("  ./archiver <archive_name> -i(--input) <file>\n");
    printf("  ./archiver <archive_name> -e(--extract) <file>\n");
    printf("  ./archiver <archive_name> -s(--stat)\n");
    printf("  ./archiver -h(--help)\n");
}

int add_file_to_archive(const char *archive_name, const char *filename) {
    // Пока просто выводим, что вызвали
    printf("Добавляем файл '%s' в архив '%s'\n", filename, archive_name);
    return 0;
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

    if (argc < 4) {
    fprintf(stderr, "Не хватает аргументов для -i\n");
    print_help();
    return 1;
    }

    char *archive = argv[1];
    char *flag = argv[2];
    char *file = argv[3];

    if (strcmp(flag, "-i") == 0 || strcmp(flag, "--input") == 0) {
        return add_file_to_archive(archive, file);
    }
    

    // Пока не поддерживаем другие команды
    fprintf(stderr, "Error: unknown command\n");
    print_help();
    return 1;
}
