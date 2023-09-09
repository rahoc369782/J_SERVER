#include "./symboltable.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../../status.h"
#include "../../utils/memory/p_memory.h"
#include "../../utils/common/customstring.h"

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
        TYPE_KEY_HEX,
        TYPE_VALUE_HEX};

    for (int i = 0; i < sizeof(symbolArray); i++)
    {
        struct symbol *symbol = pmalloc(sizeof(struct symbol));
        symbol->token = symbolArray[i];
        switch (symbolArray[i])
        {
        case LEFT_SQUARE_BRACKET:
            symbol->next_expected = NEWLINE_CHARACTER;
            break;
        case RIGHT_SQUARE_BRACKET:
            symbol->next_expected = '\0';
            break;
        case LEFT_CURLY_BRACE:
            symbol->next_expected = SPACE_CHARACTER;
            break;
        case RIGHT_CURLY_BRACE:
            symbol->next_expected = NEWLINE_CHARACTER;
            break;
        case SPACE_CHARACTER:
            symbol->next_expected = NEWLINE_CHARACTER;
            break;
        case TYPE_KEY_HEX:
            symbol->next_expected = COLON_CHARACTER;
            break;
        case COLON_CHARACTER:
            symbol->next_expected = VALUE_HEX;
            break;
        case TYPE_VALUE_HEX:
            symbol->next_expected = SPACE_CHARACTER;
            break;
        case TYPE_VALUE_HEX:
            symbol->next_expected = SPACE_CHARACTER;
            break;
        default:
            break;
        }
    }
}
