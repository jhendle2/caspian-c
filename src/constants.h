#ifndef CASPIAN_CONSTANTS_H
#define CASPIAN_CONSTANTS_H

#include "lexer.h"

// bool isNumericConst(const Token* token);
bool isIntegerConst(const Token* token);
bool isFloatConst  (const Token* token);
bool isHexConst    (const Token* token);
bool isDecConst    (const Token* token);
bool isCharConst   (const Token* token);
bool isStringConst (const Token* token);

#endif /* CASPIAN_CONSTANTS_H */