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

#define CASPIAN_MAX_ASM_CONSTANTS 64
typedef struct {
    uint id;
    enum AsmConstantType {
        ACT_BYTE , /* db */
        ACT_WORD , /* dw */
        ACT_DWORD, /* dd (0x12345678) */
        ACT_QWORD, /* dq (double-precision float) */

        /* These do not accept integer constants as inputs (MASM?) */
        ACT_TWORD, /* dt (extended-precision float) */
        ACT_OWORD, /* do */
        ACT_YWORD, /* dy */
        ACT_ZWORD, /* dz */
    } type;
    Token token;
    char data_identifier[32];
} AsmConstant;

extern AsmConstant gAsmConstantTable[CASPIAN_MAX_ASM_CONSTANTS];
extern uint gAsmConstantTableSz;
void addAsmConstant(const AsmConstant* asm_const);
bool getAsmConstant(const Token* token, AsmConstant* asm_const);

#endif /* CASPIAN_CONSTANTS_H */