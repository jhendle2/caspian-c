#ifndef CASPIAN_PARSER_H
#define CASPIAN_PARSER_H

#include "lexer.h"

#define CASPIAN_MAX_SYNTAX_CHILDREN 512

typedef struct syntax_node_s {
    uint num_tokens, num_children;
    Token tokens[CASPIAN_MAX_TOKENS_IN_LINE];

    struct syntax_node_s* parent;
    struct syntax_node_s* children[CASPIAN_MAX_SYNTAX_CHILDREN];
} *SyntaxPtr;

SyntaxPtr newSyntaxPtr  (const Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], const uint num_tokens);
void      delSyntaxPtr  (SyntaxPtr* sp);
void      printSyntaxPtr(const SyntaxPtr sp);
void      treeSyntaxPtr (const SyntaxPtr sp, const uint level);
#define masterTreeSyntaxPtr(MASTER) treeSyntaxPtr(MASTER, 0)

SyntaxPtr buildSyntaxTree(const char* file_path, const FileLine file_as_lines[CASPIAN_MAX_LINES_IN_FILE], const uint num_file_lines);

#endif /* CASPIAN_PARSER_H */