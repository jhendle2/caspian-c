#include "constants.h"

#include <string.h>

#include "utils.h"
#include "error.h"

AsmConstant gAsmConstantTable[CASPIAN_MAX_ASM_CONSTANTS];
uint gAsmConstantTableSz = 0;

void addAsmConstant(const AsmConstant* asm_const) {
    if (gAsmConstantTableSz == CASPIAN_MAX_ASM_CONSTANTS) {
        warning_token(asm_const->token, "Maximum ASM constants is currently set to "MAGENTA"(%u)"RESET"\n", CASPIAN_MAX_ASM_CONSTANTS);        
        return;
    }
    gAsmConstantTable[gAsmConstantTableSz++] = (*asm_const);
}

bool getAsmConstant(const Token* token, AsmConstant* asm_const) {
    for (uint i = 0; i<gAsmConstantTableSz; i++) {
        if (cmpTokens(token, &(gAsmConstantTable[i].token))) {
            (*asm_const) = gAsmConstantTable[i];
            return true;
        }
    } return false;
}

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