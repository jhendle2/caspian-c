#include "utils.h"

#include <stdio.h>
#include <string.h>

char* lstrip(char* s) {
    char* t = s;
    while (*t && *t==' ') t++;
    if (!*t) return s;
    return t;
}
void replace(char* s, const char original, const char replacement) {
    char* t = s;
    while (*t++) {
        if (*t==original) *t=replacement;
    }
}
bool empty       (const char* s) { return !*s; }
bool isWhitespace(const char c)  { return c==' ' || c=='\t' || c=='\n' || c=='\r'; }