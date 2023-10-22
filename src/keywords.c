#include "keywords.h"

bool isConditional(const Token* token) {
    return (
        cmpToken(token, "else")   ||
        cmpToken(token, "if")     ||
        cmpToken(token, "switch")
    );
}

bool isLoop(const Token* token) {
    return (
        cmpToken(token, "for")    ||
        cmpToken(token, "while")
    );
}

bool isKeyword(const Token* token) {
    return (
        cmpToken(token, "auto") ||
        cmpToken(token, "break") ||
        cmpToken(token, "case") ||
        cmpToken(token, "char") ||
        cmpToken(token, "const") ||
        cmpToken(token, "continue") ||
        cmpToken(token, "default") ||
        cmpToken(token, "do") ||
        cmpToken(token, "double") ||
        cmpToken(token, "else") ||
        cmpToken(token, "enum") ||
        cmpToken(token, "extern") ||
        cmpToken(token, "float") ||
        cmpToken(token, "for") ||
        cmpToken(token, "goto") ||
        cmpToken(token, "if") ||
        cmpToken(token, "inline") ||
        cmpToken(token, "int") ||
        cmpToken(token, "long") ||
        cmpToken(token, "register") ||
        cmpToken(token, "restrict") ||
        cmpToken(token, "return") ||
        cmpToken(token, "short") ||
        cmpToken(token, "signed") ||
        cmpToken(token, "sizeof") ||
        cmpToken(token, "static") ||
        cmpToken(token, "struct") ||
        cmpToken(token, "switch") ||
        cmpToken(token, "typedef") ||
        cmpToken(token, "union") ||
        cmpToken(token, "unsigned") ||
        cmpToken(token, "void") ||
        cmpToken(token, "volatile") ||
        cmpToken(token, "while")
    );
}