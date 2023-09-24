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
    // const FileLine* origin;
    FileLine origin;
    char            text[CASPIAN_MAX_TOKEN_SZ];
} Token;

Token newToken    (const uint offset, const FileLine* origin, const char* text);
void  printToken  (const Token* token);
bool  cmpToken    (const Token* token, const char* str);
bool  cmpTokens   (const Token* token, const Token* other);
uint  tokenizeLine(const FileLine* fl, Token tokens[CASPIAN_MAX_TOKENS_IN_LINE]);

void  printTokens (const Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], const uint len);
uint  copyTokens  (Token a[CASPIAN_MAX_TOKENS_IN_LINE], const Token b[CASPIAN_MAX_TOKENS_IN_LINE], const uint len);
uint  appendTokens(Token a[CASPIAN_MAX_TOKENS_IN_LINE], const uint a_len, const Token b[CASPIAN_MAX_TOKENS_IN_LINE], const uint b_len);
#define foreachToken(ITERATOR, TOKENS, NUM_TOKENS) Token ITERATOR = TOKENS[0]; for (uint INDEX = 0; INDEX<NUM_TOKENS; ITERATOR = TOKENS[INDEX++])

#define frontToken(TOKENS)     TOKENS[0    ]
#define backToken(TOKENS, LEN) TOKENS[LEN-1]

#define MASTER_TOKEN_STR "#MASTER"
#define generateMasterTokens(FILE_PATH, NAME) \
    const FileLine MASTER_LINE  = newFileLine(0,  FILE_PATH  , MASTER_TOKEN_STR);\
    const Token    MASTER_TOKEN = newToken   (0, &MASTER_LINE, MASTER_TOKEN_STR);\
    const Token    NAME[CASPIAN_MAX_TOKENS_IN_LINE] = {MASTER_TOKEN};

#endif /* CASPIAN_LEXER_H */