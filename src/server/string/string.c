#include <stdio.h>

void convertto_num(char *port)
{
    short i = 0;
    while (port[i] != 0)
    {
        printf("%d", port[i] - 48);
        i++;
    }
}