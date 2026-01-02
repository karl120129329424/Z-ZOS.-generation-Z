#include <stdio.h>
int main(void)
{
    char buffer[1024];
    while(fgets(buffer, sizeof(buffer), stdin) != NULL)
    {
        fputs(buffer, stdout);
    }
    return 0;
}