#ifndef CASPIAN_PARSER2_H
#define CASPIAN_PARSER2_H

#include <stdbool.h>

#include "lexer.h"

#define CASPIAN_MAX_SYNTAX_CHILDREN 512
#define CASPIAN_MAX_SYNTAX_DEPTH    32
// #define CASPIAN_MAX_SYNTAX_DEPTH    CASPIAN_MAX_TOKENS_IN_LINE

typedef struct SyntaxNode {
    uint num_tokens, num_children;
    Token tokens[CASPIAN_MAX_TOKENS_IN_LINE];

    struct SyntaxNode* parent;
    struct SyntaxNode* children[CASPIAN_MAX_SYNTAX_CHILDREN];
} *SyntaxPtr;

SyntaxPtr newSyntaxPtr  (const Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], const uint num_tokens);
void      delSyntaxPtr  (SyntaxPtr* sp);
void      printSyntaxPtr(const SyntaxPtr sp);
void      treeSyntaxPtr (const SyntaxPtr sp, const uint level);

SyntaxPtr buildSyntaxTree(const char* file_path, const FileLine file_as_lines[CASPIAN_MAX_LINES_IN_FILE], const uint num_file_lines);

#endif /* CASPIAN_PARSER2_H */