#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

// Structs prototypes required in symbol table

struct symbol
{
    char token;
    char *desc;
};

// Hash table size for symbol table.
#define TOKENS_COUNT 10
#define CONTEXTTABLE_SIZE 5
#define SYMBOLTBLE_SIZE (TOKENS_COUNT + 10)

// Functions prototypes.
struct symbol **init_symboltable(void);
void context_table(void);
int check_token_incontext_table(unsigned short type, char s);

// struct symbol *search_table(void);
#endif