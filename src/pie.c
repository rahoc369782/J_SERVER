#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>
#include "server/syscalls/p_server.h"
#include "connections/clients.h"
#include "status.h"
#include "./utils/memory/p_memory.h"
#include "pie.h"
#include "parser/parser.h"
#include "./utils/heap_usage/h_usage.h"

int main(int args, char *agrv[])
{
    if (args < 2)
    {
        printf("Minimum three arguments expected");
        return -EARGS;
    }
    parser_main();
    int pie_usage = initiate_pieh_calc();
    if (pie_usage < 0)
    {
        printf("Failed to initiate heap.");
        return pie_usage;
    };

    struct dependancy *depn;
    depn = pmalloc(sizeof(struct dependancy));
    int *clients = c_clinet();
    if (clients == NULL)
    {
        printf("Third party clients connections are not created. Something went wrong.\n");
        return -PIE_OK;
    }
    depn->c_conn = clients;

    int res = initialize_server(depn);

    if (res < 0)
    {
        printf("server creation failed");
        return res;
    }
    return PIE_OK;
}