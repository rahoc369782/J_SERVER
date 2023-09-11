#include <stdio.h>
#include <stdint.h>
#include "parser.h"
#include "./tokenization/tokenization.h"

/*

Author : Rahul Darekar
Date : 29-08-2023

Parser is for parsing PIE's requested queries.

PIE is using serializing protocol. Sequence with proper data and input matter.
Following commands are allowing for PIEv.0.0.1

*/
void init_parser()
{
    printf("Initializing parser\n");
    commands_parser();
}