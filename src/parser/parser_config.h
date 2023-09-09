#ifndef PARSERCONFIG_H
#define PARSERCONFIG_H

// Hash table size for symbol table.
#define TOKENS_COUNT 10
#define SYMBOLTBLE_SIZE (TOKENS_COUNT + 10)

// Required tokens as per syntax defination of parsers  
#define LEFT_SQUARE_BRACKET    0x5B
#define LEFT_CURLY_BRACE       0x7B
#define COMMA_CHARACTER        0x2C
#define COLON_CHARACTER        0x3A
#define SPACE_CHARACTER        0x20
#define NEWLINE_CHARACTER      0x0A
#define RIGHT_CURLY_BRACE      0x7D
#define RIGHT_SQUARE_BRACKET   0x5D


#endif