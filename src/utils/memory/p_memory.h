#ifndef MEMORY_H

#define MEMORY_H
#include <stdlib.h>
// custom malloc for our application use
void *pmalloc(size_t size);
void *pcalloc(size_t ele, int ele_size);
short pfree(void *);

#endif