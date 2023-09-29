#ifndef CASPIAN_UTILS_H
#define CASPIAN_UTILS_H

/* String Utils */
char* lstrip(char* s);
void  replace(char* s, const char original, const char replacement);
bool  empty(const char* s);
bool  isWhitespace(const char c);

/* Token Utils */
#include "lexer.h"
bool isAlpha(const char c);
bool isDigit(const char c);
bool isInteger(const Token* token);

#endif /* CASPIAN_UTILS_H */