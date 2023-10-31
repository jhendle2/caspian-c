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

Token repairAsmStringConst(const Token* string_const) {
    printf(MAGENTA"REPAIRING...\n"RESET);
    Token repaired = newToken(string_const->offset, &(string_const->origin), "");
    uint repaired_index = 0;

    #define appendChar(C) {\
        repaired.text[repaired_index++]=C;\
    }

    for (uint i = 0; i<(CASPIAN_MAX_TOKEN_SZ-1); i++) {
        const char c = string_const->text[i];
        if (c == 0) break;

        const char d = string_const->text[i+1];
        if (c == '\\') {
            appendChar('\"');
            appendChar(',');
            appendChar(' ');
            switch (d) {
                // FIXME: So unbelievably sloppy - You should be ashamed of yourself!
                case 'a' : appendChar('0'); appendChar('x'); appendChar('0'); appendChar('7'); break;
                case 'b' : appendChar('0'); appendChar('x'); appendChar('0'); appendChar('8'); break;
                case 'e' : appendChar('0'); appendChar('x'); appendChar('1'); appendChar('B'); break;
                case 'f' : appendChar('0'); appendChar('x'); appendChar('0'); appendChar('C'); break;
                case 'n' : appendChar('0'); appendChar('x'); appendChar('0'); appendChar('A'); break;
                case 'r' : appendChar('0'); appendChar('x'); appendChar('0'); appendChar('D'); break;
                case 't' : appendChar('0'); appendChar('x'); appendChar('0'); appendChar('9'); break;
                case 'v' : appendChar('0'); appendChar('x'); appendChar('0'); appendChar('B'); break;
                case '\\': appendChar('0'); appendChar('x'); appendChar('5'); appendChar('C'); break;
                case '\'': appendChar('0'); appendChar('x'); appendChar('2'); appendChar('7'); break;
                case '\"': appendChar('0'); appendChar('x'); appendChar('2'); appendChar('2'); break;
                default:
                    error_token(1, *string_const, "Not a recognized escape sequence "MAGENTA"`%c%c`"RESET, c, d);
                    break;
            }
            i++;
            const char e = string_const->text[i+1];
            if (e != '\"') { /* There's more left in the string */
                appendChar(',');
                appendChar(' ');
                appendChar('\"');
            } else i++; /* So we skip the final close quote if the string constant ended with an escape sequence */
            continue;
        }
        else appendChar(c);
    }
    printf(MAGENTA"REPAIRED=%s\n"RESET, repaired.text);
    return repaired;
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