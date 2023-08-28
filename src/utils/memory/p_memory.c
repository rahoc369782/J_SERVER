#include <stdlib.h>
#include <stdlib.h>
#include "../heap_usage/h_usage.h"

void *pmalloc(size_t size)
{
    void *ptr = malloc(size);
    add_alloc(size);
    return ptr;
}

void pfree(void *ptr)
{
    free(ptr);
}