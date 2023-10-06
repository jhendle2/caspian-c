#ifndef CASPIAN_LEXER_H
#define CASPIAN_LEXER_H

#include <stdbool.h>
#include "common.h"

/****************************************************************************************************/
#define CASPIAN_MAX_PATH_SZ       64
#define CASPIAN_MAX_FILELINE_SZ   512
#define CASPIAN_MAX_LINES_IN_FILE 1024

#ifndef DISABLE_PREPROCESSOR
    #define CASPIAN_DISABLE_PREPROCESSOR true
#else
    #define CASPIAN_DISABLE_PREPROCESSOR false
#endif

typedef struct {
    uint line_number;
    char origin[CASPIAN_MAX_PATH_SZ];
    char text  [CASPIAN_MAX_FILELINE_SZ];
} FileLine;

FileLine newFileLine    (const uint line_number, const char* origin, const char* text);
void     printFileLine  (const FileLine* fl);
uint     readFileAsLines(const char* file_path, FileLine file_as_lines[CASPIAN_MAX_LINES_IN_FILE]);

/****************************************************************************************************/
#define CASPIAN_MAX_TOKEN_SZ       256
#define CASPIAN_MAX_TOKENS_IN_LINE 128

typedef struct {
    uint            offset;
    // const FileLine* origin;
    FileLine origin;
    char            text[CASPIAN_MAX_TOKEN_SZ];
} Token;

Token newToken      (const uint offset, const FileLine* origin, const char* text);
void  printToken    (const Token* token);
bool  cmpToken      (const Token* token, const char* str);
bool  cmpTokens     (const Token* token, const Token* other);
#define tokenMatch    true
#define tokenMismatch false
uint  tokenizeLine  (const FileLine* fl, Token tokens[CASPIAN_MAX_TOKENS_IN_LINE]);

void  printTokensNoNewline(const Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], const uint len);
void  printTokens   (const Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], const uint len);
uint  copyTokens    (Token a[CASPIAN_MAX_TOKENS_IN_LINE], const Token b[CASPIAN_MAX_TOKENS_IN_LINE], const uint len);
uint  moveTokens    (Token a[CASPIAN_MAX_TOKENS_IN_LINE], Token b[CASPIAN_MAX_TOKENS_IN_LINE], const uint len);

uint  appendTokens  (Token a     [CASPIAN_MAX_TOKENS_IN_LINE], const uint a_len, const Token b[CASPIAN_MAX_TOKENS_IN_LINE], const uint b_len);
void  splitTokens   (Token input [CASPIAN_MAX_TOKENS_IN_LINE], const uint input_len, const uint split_index,
                     Token left  [CASPIAN_MAX_TOKENS_IN_LINE], uint* left_len,
                     Token right [CASPIAN_MAX_TOKENS_IN_LINE], uint* right_len);
Token popFrontTokens(Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], uint* num_tokens);
uint  pushBackTokens(Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], uint num_tokens, const Token* push_token);

#define NOT_FOUND -1
int  findToken      (Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], const uint num_tokens, const char* find);

#define foreachToken(ITERATOR, TOKENS, NUM_TOKENS) Token ITERATOR = TOKENS[0]; for (uint INDEX = 0; INDEX<NUM_TOKENS; ITERATOR = TOKENS[INDEX++])

#define frontToken(TOKENS)     TOKENS[0    ]
#define backToken(TOKENS, LEN) TOKENS[LEN-1]

#define MASTER_TOKEN_STR "#MASTER"
#define generateMasterTokens(FILE_PATH, NAME) \
    const FileLine MASTER_LINE  = newFileLine(0,  FILE_PATH  , MASTER_TOKEN_STR);\
    const Token    MASTER_TOKEN = newToken   (0, &MASTER_LINE, MASTER_TOKEN_STR);\
    const Token    NAME[CASPIAN_MAX_TOKENS_IN_LINE] = {MASTER_TOKEN};
#define generateMasterToken(FILE_PATH, NAME) \
    const FileLine MASTER_LINE  = newFileLine(0,  FILE_PATH  , FILE_PATH);\
    const Token    MASTER_TOKEN = newToken   (0, &MASTER_LINE, FILE_PATH);\
    const Token    NAME[CASPIAN_MAX_TOKENS_IN_LINE] = {MASTER_TOKEN};

bool isOperatorDelimiter(const Token* token);

#endif /* CASPIAN_LEXER_H */