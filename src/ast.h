#ifndef CASPIAN_AST_H
#define CASPIAN_AST_H

#include "lexer.h"
#include "parser.h"

#define CASPIAN_MAX_AST_CHILDREN 512

typedef struct ast_node_s {
    uint num_tokens, num_children;
    Token tokens[CASPIAN_MAX_TOKENS_IN_LINE];

    struct ast_node_s* parent;
    struct ast_node_s* children[CASPIAN_MAX_AST_CHILDREN];
} *AstPtr;

AstPtr newAstPtr  (const Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], const uint num_tokens);
void   delAstPtr  (AstPtr* sp);
void   printAstPtr(const AstPtr sp);
void   treeAstPtr (const AstPtr sp, const uint level);
#define masterTreeAstPtr(MASTER) treeAstPtr(MASTER, 0)

AstPtr buildAstTree(const SyntaxPtr master_sp);

#endif /* CASPIAN_PARSER_H */