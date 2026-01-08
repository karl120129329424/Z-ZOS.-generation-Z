#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include <pwd.h>
#include <grp.h> 

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#include <time.h>
//макось, мммммм

//
char *get_username(uid_t uid)
{
    struct passwd *pw = getpwuid(uid);
    return pw ? pw->pw_name : "unknown";
}

char *get_groupname(gid_t gid)
{
    struct group *gr = getgrgid(gid);
    return gr ? gr->gr_name : "unknown";
}
//

//функция для формирования строки прав доступа (drwxr-xr--)
void print_permissions(mode_t mode, char *buf) {
    buf[0] = '-';

    // тип файла
    if (S_ISDIR(mode))      buf[0] = 'd';
    else if (S_ISLNK(mode)) buf[0] = 'l';
    else if (S_ISCHR(mode)) buf[0] = 'c';
    else if (S_ISBLK(mode)) buf[0] = 'b';
    else if (S_ISFIFO(mode))buf[0] = 'p';
    else if (S_ISSOCK(mode))buf[0] = 's';

    // права владельца
    buf[1] = (mode & S_IRUSR) ? 'r' : '-';
    buf[2] = (mode & S_IWUSR) ? 'w' : '-';
    buf[3] = (mode & S_IXUSR) ? 'x' : '-';

    // группы
    buf[4] = (mode & S_IRGRP) ? 'r' : '-';
    buf[5] = (mode & S_IWGRP) ? 'w' : '-';
    buf[6] = (mode & S_IXGRP) ? 'x' : '-';

    // остальные
    buf[7] = (mode & S_IROTH) ? 'r' : '-';
    buf[8] = (mode & S_IWOTH) ? 'w' : '-';
    buf[9] = (mode & S_IXOTH) ? 'x' : '-';
    buf[10] = '\0';
}

char* format_time(time_t t) {
    static char buf[20];
    struct tm *tm_info = localtime(&t);
    strftime(buf, sizeof(buf), "%b %d %H:%M", tm_info);
    return buf;
}

#define COLOR_RESET "\033[0m"
#define COLOR_BLUE  "\033[34m"   // директории
#define COLOR_GREEN "\033[32m"   // исполняемые
#define COLOR_CYAN  "\033[36m"   // ссылки

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    int flag_a = 0;
    int flag_l = 0;

    int opt;
    while ((opt = getopt(argc, argv, "la")) != -1)
    {
        switch (opt)
        {
            case 'a':
                flag_a = 1;
                break;
            case 'l':
                flag_l = 1;
                break;
            default:
                fprintf(stderr, "Использование: %s [-la] [папка]\n", argv[0]);
                return 1;
        }
    }

    const char *path = ".";
    if (optind < argc) {
        path = argv[optind];
    }

    // откр директорию
    DIR *dir = opendir(path);
    if (!dir) {
        perror(path);
        return 1;
    }

    // сбор имён в массив
    char **file_list = NULL;   // массив строк (указателей на char)
    int file_count = 0;        // кол-во скок добавили
    int alloc_size = 0;        // скок памяти выделено (в элементах)
    //-

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (!flag_a && entry->d_name[0] == '.'){continue;}

        // нужно ли расширить массив?
        if (file_count >= alloc_size) {
            alloc_size = alloc_size ? alloc_size * 2 : 8;
            file_list = realloc(file_list, alloc_size * sizeof(char *));
            if (!file_list) {
                perror("realloc");
                exit(1);
            }
        }

        file_list[file_count] = strdup(entry->d_name); // копируем имя (strdup = malloc + strcpy)
        if (!file_list[file_count])
        {
            perror("strdup");
            exit(1);
        }
        file_count++;
    }

    closedir(dir);


    // Сортируем
    for (int i = 0; i < file_count - 1; i++)
    {
        for (int j = i + 1; j < file_count; j++)
        {
            if (strcmp(file_list[i], file_list[j]) > 0)
            {
                char *tmp = file_list[i];
                file_list[i] = file_list[j];
                file_list[j] = tmp;
            }
        }
    }

    // цикл вывода
    for (int i = 0; i < file_count; i++) {
        if (flag_l) {
            char full_path[PATH_MAX];
            snprintf(full_path, sizeof(full_path), "%s/%s", path, file_list[i]);

            struct stat st;
            if (lstat(full_path, &st) == 0) {
                // подготов строку прав
                char perms[11];
                print_permissions(st.st_mode, perms);

                // получ имена
                char *user = get_username(st.st_uid);
                char *group = get_groupname(st.st_gid);

                // Определяем цвет
                const char *color = COLOR_RESET;

                if (S_ISDIR(st.st_mode)) {color = COLOR_BLUE;}

                else if (S_ISLNK(st.st_mode)) {color = COLOR_CYAN;}
                    
                else if (st.st_mode & S_IXUSR) {color = COLOR_GREEN;}

                printf("%s%s %lu %s %s %lu %s %s%s\n",
                       color,
                       perms,
                       (unsigned long)st.st_nlink,
                       user,
                       group,
                       (unsigned long)st.st_size,
                       format_time(st.st_mtime),
                       file_list[i],
                       COLOR_RESET);
            }
            else {perror(full_path);}

        } else {printf("%s\n", file_list[i]);}
    }

    // Очистка памяти
    for (int i = 0; i < file_count; i++) {
        free(file_list[i]);
    }
    free(file_list);
    //-

    return 0;
}