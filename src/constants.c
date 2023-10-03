#include "constants.h"

#include <string.h>

#include "utils.h"

bool isIntegerConst(const Token* token) {
    return (isDecConst(token) || isHexConst(token));
}
bool isFloatConst  (const Token* token) {
    const char* text = token->text;
    const char* iter = text;
    uint decimal_count = 0;
    if (*iter == '-') iter++;
    while (*iter) {
        if (*iter == '.') decimal_count++;
        else if (!isDec(*iter)) return false;
        iter++;
    }
    return (decimal_count <= 1);
}
bool isHexConst    (const Token* token) {
    const char* text = token->text;
    const char* iter = text;
    // if (*iter == '-') iter++; /* Idk if this is supported? */
    if (!(*iter == '0' && *(iter+1) == 'x')) return false;
    while (*iter) {
        if (isHex(*iter)==false) return false;
        iter++;
    } return true;
}
bool isDecConst    (const Token* token) {
    const char* text = token->text;
    const char* iter = text;
    if (*iter == '-') iter++;
    while (*iter) {
        if (isDec(*iter)==false) return false;
        iter++;
    } return true;
}
bool isCharConst   (const Token* token) {
    const char* text = token->text;
    return (
        text[0] == '\'' &&
        text[strlen(text)-1] == '\''
    );

}
bool isStringConst (const Token* token) {
    const char* text = token->text;
    return (
        text[0] == '\"' &&
        text[strlen(text)-1] == '\"'
    );
}