#include "customstring.h"

short isnumeric(char s)
{
    if (s >= 48 && s<= 57) {
        return 1;
    }
    return 0;
}