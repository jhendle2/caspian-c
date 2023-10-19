#include "utils.h"

#include <stdio.h>
#include <string.h>

/* String Utils */
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

// bool isAlpha(const char c) {
//     return (
//         (c >= 'A' && c <= 'Z') ||
//         (c >= 'a' && c <= 'z')
//     );
// }

// bool isDigit(const char c) {
//     return (c >= '0' && c <= '9');
// }

/* Token Utils */
bool isInteger(const Token* token) {
    const uint len = strlen(token->text);
    bool is_negative = (token->text[0] == '-');
    if (is_negative && len == 1) return false;
    for (uint i = (is_negative ? 1 : 0); i<len; i++) {
        if (!isDec(token->text[i])) return false;
    } return true;
}

bool isFloat(const Token* token) {
    const uint len = strlen(token->text);
    bool saw_dot = false;
    bool is_negative = (token->text[0] == '-');
    if (is_negative && len == 1) return false;
    for (uint i = (is_negative ? 1 : 0); i<len; i++) {
        if (token->text[i]=='.') {
            if (saw_dot) return false;
            saw_dot = true;
            continue;
        }
        if (!isDec(token->text[i])) return false;
    } return saw_dot;
}
bool isString(const Token* token) {
    return (
        token->text[0] == '\"' &&
        token->text[strlen(token->text)-1] == '\"'
    );
}
bool isCharacter(const Token* token) {
    return (
        token->text[0] == '\'' &&
        token->text[strlen(token->text)-1] == '\''
    );
}