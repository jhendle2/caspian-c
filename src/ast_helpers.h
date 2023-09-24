#ifndef CASPIAN_AST_HELPERS_H
#define CASPIAN_AST_HELPERS_H

#include "parser.h"

#define AST_VERIFY(NAME) \
    bool AST_VERIFY_##NAME(const SyntaxPtr sp)

AST_VERIFY(FunctionHeader);
AST_VERIFY(Typedef);

#endif /* CASPIAN_AST_HELPERS_H */