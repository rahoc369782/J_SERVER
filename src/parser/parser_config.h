#ifndef PARSERCONFIG_H
#define PARSERCONFIG_H

// Required tokens as per syntax definition of parsers
#define LEFT_SQUARE_BRACKET 0x5B  // Decimal: 91   [ Character
#define LEFT_CURLY_BRACE 0x7B     // Decimal: 123  { Character
#define COMMA_CHARACTER 0x2C      // Decimal: 44   , Character
#define COLON_CHARACTER 0x3A      // Decimal: 58   : Character
#define SPACE_CHARACTER 0x20      // Decimal: 32   Space Character
#define NEWLINE_CHARACTER 0x0A    // Decimal: 10   Newline Character
#define RIGHT_CURLY_BRACE 0x7D    // Decimal: 125  } Character
#define RIGHT_SQUARE_BRACKET 0x5D // Decimal: 93   ] Character
#define TYPE_CHUNK_HEX 0x6B       // Decimal: 107  k Character
#define TYPE_VALUE_HEX 0x76       // Decimal: 118  v Character

#endif