#ifndef CASPIAN_ASSEMBLER_H
#define CASPIAN_ASSEMBLER_H

#include <stdio.h>

#include "ast.h"

extern FILE* gActiveAsm; 
void assembleFromAST(FILE* asm_out, const AstPtr master);

#endif /* CASPIAN_ASSEMBLER_H */