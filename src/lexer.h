#ifndef CASPIAN_LEXER_H
#define CASPIAN_LEXER_H

#include "common.h"

/****************************************************************************************************/
#define CASPIAN_MAX_PATH_SZ       64
#define CASPIAN_MAX_FILELINE_SZ   128
#define CASPIAN_MAX_LINES_IN_FILE 1024

typedef struct {
    uint line_number;
    char origin[CASPIAN_MAX_PATH_SZ];
    char text  [CASPIAN_MAX_FILELINE_SZ];
} FileLine;

FileLine newFileLine    (const uint line_number, const char* origin, const char* text);
void     printFileLine  (const FileLine* fl);
uint     readFileAsLines(const char* file_path, FileLine file_as_lines[CASPIAN_MAX_LINES_IN_FILE]);

/****************************************************************************************************/
#define CASPIAN_MAX_TOKEN_SZ       32
#define CASPIAN_MAX_TOKENS_IN_LINE 256

typedef struct {
    uint            offset;
    const FileLine* origin;
    char            text[CASPIAN_MAX_TOKEN_SZ];
} Token;

Token newToken    (const uint offset, const FileLine* origin, const char* text);
void  printToken  (const Token* token);
uint  tokenizeLine(const FileLine* fl, Token tokens[CASPIAN_MAX_TOKENS_IN_LINE]);

#endif /* CASPIAN_LEXER_H */