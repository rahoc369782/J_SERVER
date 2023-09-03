/*
    Author name : Rahul Darekar
    Date : 23-08-2023

    # This module is created for measuring heap usage and memory allocation by PIE.
    # Also it will calculate how much memory is in use and how many objects are there in programes.

*/
#include <stdio.h>
#include "h_usage.h"
#include "../memory/p_memory.h"
#include "../../config.h"
#include <stdlib.h>
/*
    Because memory allocation is in bytes. We will returns all values in total bytes.

*/
h_usage *pie_h;

void add_alloc(size_t size)
{
    printf("s added is %ld\n", size);
    pie_h->alloc += size;
    printf("size is %ld\n", pie_h->alloc);
    return;
}

void sub_alloc(size_t size)
{
}

int initiate_pieh_calc()
{
    printf("called i \n");
    pie_h = malloc(sizeof(h_usage));
    pie_h->alloc = 0;
    pie_h->dead_obje = 0;
    pie_h->live_obje = 0;
    if (pie_h == NULL)
    {
        return -EHCALC;
    }
    add_alloc(sizeof(h_usage));
    return EHCALC;
}
