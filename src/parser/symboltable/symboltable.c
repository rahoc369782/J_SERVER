#include "./symboltable.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../../status.h"
#include "../../utils/memory/p_memory.h"
#include "../../utils/common/customstring.h"
#include "../../parser/parser_config.h"

// Global table for context checker

struct symbol **context_table;
char start_token[CONTEXTTABLE_SIZE];
char object_token[CONTEXTTABLE_SIZE];

static int hash(const char key, int size)
{
    return key % size;
}
// This function will enable symbol table by allocationg memory and setting required tokens into it.
struct symbol **init_symboltable(void)
{
    struct symbol **symbol_table = pmalloc(SYMBOLTBLE_SIZE * sizeof(struct symbol *));
    if (!symbol_table)
    {
        return NULL;
    }

    // Symbols which we required for grammer validation.
    char symbolArray[] = {
        LEFT_SQUARE_BRACKET,
        LEFT_CURLY_BRACE,
        COMMA_CHARACTER,
        COLON_CHARACTER,
        SPACE_CHARACTER,
        NEWLINE_CHARACTER,
        RIGHT_CURLY_BRACE,
        RIGHT_SQUARE_BRACKET,
        TYPE_CHUNK_HEX,
        TYPE_VALUE_HEX};

    for (int i = 0; i < sizeof(symbolArray); i++)
    {
        struct symbol *symbol = pmalloc(sizeof(struct symbol));
        symbol->token = symbolArray[i];
    }
}

// This function will enable whether char like space or null terminator is allowed in particular context like ([] or {})
struct symbol *search(int key)
{
    // get the hash
    int hashval = hash(key);

    // move in array until an empty
    while (context_table[hashval] != NULL)
    {

        if (context_table[hashval]->key == key)
            return context_table[hashval];

        // go to next cell
        ++hashval;

        // wrap around the table
        hashval %= SIZE;
    }

    return NULL;
}

int check_token_incontext_table(unsigned short type, char s)
{
    char *tokens;
    if (type == LEFT_SQUARE_BRACKET)
    {
        tokens = start_token;
    }
    else
    {
        tokens = object_token;
    }

    for (size_t i = 0; i < CONTEXTTABLE_SIZE; i++)
    {
        if (tokens[i] == s)
        {
            return tokens[i];
        }
    };
    return 0;
}

void insert(struct symbol *symbol, int hash, int size)
{
    // move in array until an empty or deleted cell
    while (context_table[hash] != NULL)
    {
        // go to next cell
        ++hash;

        // wrap around the table
        hash %= size;
    }

    context_table[hash] = symbol;
}
void context_table(void)
{

    memset(start_token, 0, CONTEXTTABLE_SIZE);
    memset(object_token, 0, CONTEXTTABLE_SIZE);

    start_token[0] = NEWLINE_CHARACTER;
    object_token[0] = SPACE_CHARACTER;
    // context_table = pmalloc(CONTEXTTABLE_SIZE * sizeof(struct symbol *));
    // if (!context_table)
    // {
    //     return NULL;
    // }

    // char symbolArray[] = {NEWLINE_CHARACTER};

    // for (int i = 0; i < sizeof(symbolArray); i++)
    // {
    //     struct symbol *symbol = pmalloc(sizeof(struct symbol));
    //     symbol->token = symbolArray[i];

    //     // Add in symbol table
    //     int hashval = hash(symbolArray[i], CONTEXTTABLE_SIZE);

    //     // Insert in hash table
    //     insert(context_table, symbol, hashval, CONTEXTTABLE_SIZE);
    // }
}