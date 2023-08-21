#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "server/syscalls/p_server.h"
#include "status.h"

int main(int args, char *agrv[])
{
    if (args < 3)
    {
        printf("Minimum three arguments expected");
        return -EARGS;
    }
    int res = initialize_server();
    if (res < 0)
    {
        printf("server creation failed");
        return res;
    }
    return PIE_OK;
}