#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>   // +
#include <string.h>

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    // По умолчанию директория "."
    const char *path = ".";
    
    // Открываем директорию
    DIR *dir = opendir(path);
    if (!dir)
    {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;
    
    // Читаем по одной записи
    while ((entry = readdir(dir)) != NULL) {
        // Пропускаем скрытые файлы нач с .
        if (entry->d_name[0] == '.')
        {
            continue;
        }
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
    return 0;
}