#ifndef H_USAGE_H
#define H_USAGE_H

#include <stdlib.h>
typedef struct h_usage
{
    size_t alloc;  // Number of B=byte is added each time we set memory in heap.
    int live_obje; // Count 1 added every time when new allocation is
    int dead_obje; // Count 1 subtracted each time when we free memory
} h_usage;

void add_alloc(size_t size);
int initiate_pieh_calc();

#endif