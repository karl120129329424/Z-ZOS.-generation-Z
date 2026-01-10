#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>

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
    int file_fd = open(filename, O_RDONLY);
    if (file_fd == -1) {
        perror("Ошибка открытия входного файла");
        return -1;
    }

    struct stat st;
    if (fstat(file_fd, &st) == -1) {
        perror("Ошибка fstat");
        close(file_fd);
        return -1;
    }

    int archive_fd = open(archive_name, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (archive_fd == -1) {
        perror("Ошибка открытия архива");
        close(file_fd);
        return -1;
    }

    FileHeader header;
    memset(&header, 0, sizeof(header));
    strncpy(header.magic, HEADER_MAGIC, 8);
    strncpy(header.filename, filename, 15);
    header.original_size = st.st_size;
    header.mode = st.st_mode;
    header.uid = st.st_uid;
    header.gid = st.st_gid;
    header.mtime = st.st_mtime;

    if (write(archive_fd, &header, sizeof(header)) != (ssize_t)sizeof(header)) {
        perror("Ошибка записи заголовка");
        close(file_fd);
        close(archive_fd);
        return -1;
    }

    char buffer[4096];
    ssize_t nread;
    while ((nread = read(file_fd, buffer, sizeof(buffer))) > 0) {
        if (write(archive_fd, buffer, nread) != nread) {
            perror("Ошибка записи данных");
            close(file_fd);
            close(archive_fd);
            return -1;
        }
    }

    if (nread == -1) {
        perror("Ошибка чтения данных");
        close(file_fd);
        close(archive_fd);
        return -1;
    }

    close(file_fd);
    close(archive_fd);

    printf("Файл '%s' успешно добавлен в архив '%s'\n", filename, archive_name);
    return 0;
}

int show_archive_stat(const char *archive_name) {
    int archive_fd = open(archive_name, O_RDONLY);
    if (archive_fd == -1) {
        perror("Ошибка открытия архива");
        return -1;
    }

    FileHeader header;
    off_t offset = 0;

    printf("Archive contents:\n");
    printf("%-20s %-10s %-10s %-10s\n", "Filename", "Size", "UID", "GID");

    while (1) {
        if (lseek(archive_fd, offset, SEEK_SET) == -1) {
            perror("Ошибка lseek");
            close(archive_fd);
            return -1;
        }

        ssize_t nread = read(archive_fd, &header, sizeof(header));
        if (nread == 0) {
            break;
        }
        if (nread != sizeof(header)) {
            fprintf(stderr, "Ошибка: повреждённый заголовок на позиции %lld\n", offset);
            close(archive_fd);
            return -1;
        }

        if (strncmp(header.magic, HEADER_MAGIC, 8) != 0) {
            fprintf(stderr, "Ошибка: неверная сигнатура на позиции %lld\n", offset);
            close(archive_fd);
            return -1;
        }

        printf("%-20s %-10ld %-10d %-10d\n",
               header.filename,
               (long)header.original_size,
               header.uid,
               header.gid);

        offset += sizeof(header) + header.original_size;
    }

    close(archive_fd);
    return 0;
}

int find_file_in_archive(int fd, const char *filename, off_t *start_offset, off_t *end_offset) {
    FileHeader header;
    off_t current_pos = 0;

    while (1) {
        if (lseek(fd, current_pos, SEEK_SET) == -1) {
            perror("lseek");
            return -1;
        }

        ssize_t nread = read(fd, &header, sizeof(header));
        if (nread == 0) break;
        if (nread != sizeof(header)) {
            fprintf(stderr, "Ошибка чтения заголовка на позиции %lld\n", current_pos);
            return -1;
        }

        if (strncmp(header.magic, HEADER_MAGIC, 8) != 0) {
            fprintf(stderr, "Неверная сигнатура на позиции %lld\n", current_pos);
            return -1;
        }

        if (strcmp(header.filename, filename) == 0) {
            *start_offset = current_pos;
            *end_offset = current_pos + sizeof(header) + header.original_size;
            return 0;
        }

        current_pos += sizeof(header) + header.original_size;
    }

    return 1;
}

int remove_file_from_archive(const char *archive_name, const char *filename) {
    int archive_fd = open(archive_name, O_RDWR);
    if (archive_fd == -1) {
        perror("Ошибка открытия архива для удаления");
        return -1;
    }

    off_t start_offset, end_offset;
    int found = find_file_in_archive(archive_fd, filename, &start_offset, &end_offset);
    if (found != 0) {
        if (found == 1) {
            fprintf(stderr, "Файл '%s' не найден в архиве\n", filename);
        }
        close(archive_fd);
        return -1;
    }

    off_t archive_size = lseek(archive_fd, 0, SEEK_END);
    if (archive_size == -1) {
        perror("lseek конец");
        close(archive_fd);
        return -1;
    }

    if (end_offset == archive_size) {
        if (ftruncate(archive_fd, start_offset) == -1) {
            perror("ftruncate");
            close(archive_fd);
            return -1;
        }
        close(archive_fd);
        return 0;
    }

    char buffer[4096];
    off_t src_pos = end_offset;
    off_t dst_pos = start_offset;

    while (src_pos < archive_size) {
        if (lseek(archive_fd, src_pos, SEEK_SET) == -1) {
            perror("lseek src");
            close(archive_fd);
            return -1;
        }

        ssize_t nread = read(archive_fd, buffer, sizeof(buffer));
        if (nread <= 0) break;

        if (lseek(archive_fd, dst_pos, SEEK_SET) == -1) {
            perror("lseek dst");
            close(archive_fd);
            return -1;
        }

        if (write(archive_fd, buffer, nread) != nread) {
            perror("write rewrite");
            close(archive_fd);
            return -1;
        }

        src_pos += nread;
        dst_pos += nread;
    }

    if (ftruncate(archive_fd, dst_pos) == -1) {
        perror("ftruncate final");
        close(archive_fd);
        return -1;
    }

    close(archive_fd);
    return 0;
}

int extract_file_from_archive(const char *archive_name, const char *filename) {
    int archive_fd = open(archive_name, O_RDONLY);
    if (archive_fd == -1) {
        perror("Ошибка открытия архива");
        return -1;
    }

    off_t start_offset, end_offset;
    int found = find_file_in_archive(archive_fd, filename, &start_offset, &end_offset);
    if (found != 0) {
        if (found == 1) {
            fprintf(stderr, "Файл '%s' не найден в архиве\n", filename);
        }
        close(archive_fd);
        return -1;
    }

    FileHeader header;
    if (lseek(archive_fd, start_offset, SEEK_SET) == -1) {
        perror("lseek заголовок");
        close(archive_fd);
        return -1;
    }

    if (read(archive_fd, &header, sizeof(header)) != sizeof(header)) {
        perror("read заголовок");
        close(archive_fd);
        return -1;
    }

    int out_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, header.mode & 0777);
    if (out_fd == -1) {
        perror("Ошибка создания выходного файла");
        close(archive_fd);
        return -1;
    }

    char buffer[4096];
    off_t data_start = start_offset + sizeof(header);
    off_t remaining = header.original_size;

    if (lseek(archive_fd, data_start, SEEK_SET) == -1) {
        perror("lseek данные");
        close(out_fd);
        close(archive_fd);
        return -1;
    }

    while (remaining > 0) {
        size_t to_read = ((size_t)remaining > sizeof(buffer)) ? sizeof(buffer) : (size_t)remaining;
        ssize_t nread = read(archive_fd, buffer, to_read);
        if (nread <= 0) {
            perror("read данные");
            close(out_fd);
            close(archive_fd);
            return -1;
        }
        if (write(out_fd, buffer, nread) != nread) {
            perror("write выходной файл");
            close(out_fd);
            close(archive_fd);
            return -1;
        }
        remaining -= (off_t)nread;
    }

    close(out_fd);
    close(archive_fd);

    if (chown(filename, header.uid, header.gid) == -1) {
        // Игнорируем ошибку (может требовать root)
    }

    struct utimbuf utime_buf;
    utime_buf.actime = header.mtime;
    utime_buf.modtime = header.mtime;
    utime(filename, &utime_buf);

    printf("Файл '%s' извлечён из архива '%s'\n", filename, archive_name);

    if (remove_file_from_archive(archive_name, filename) != 0) {
        fprintf(stderr, "Предупреждение: не удалось удалить файл из архива\n");
        return -1;
    }

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

    if (argc < 3) {
        fprintf(stderr, "Не хватает аргументов\n");
        print_help();
        return 1;
    }

    char *archive = argv[1];
    char *flag = argv[2];

    if (strcmp(flag, "-s") == 0 || strcmp(flag, "--stat") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Для -s не нужен дополнительный аргумент\n");
            print_help();
            return 1;
        }
        return show_archive_stat(archive);
    }

    if (argc < 4) {
        fprintf(stderr, "Не хватает аргументов для '%s'\n", flag);
        print_help();
        return 1;
    }

    char *file = argv[3];

    if (strcmp(flag, "-i") == 0 || strcmp(flag, "--input") == 0) {
        return add_file_to_archive(archive, file);
    }

    if (strcmp(flag, "-e") == 0 || strcmp(flag, "--extract") == 0) {
        return extract_file_from_archive(archive, file);
    }

    fprintf(stderr, "Ошибка: неизвестная команда '%s'\n", flag);
    print_help();
    return 1;
}