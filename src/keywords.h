#ifndef CASPIAN_KEYWORDS_H
#define CASPIAN_KEYWORDS_H

#include "parser2.h"

/**************************************************************************/

enum Keywords {
    KW_AUTO,
    KW_BREAK,
    KW_CASE,
    KW_CHAR,
    KW_CONST,
    KW_CONTINUE,
    KW_DEFAULT,
    KW_DO,
    KW_DOUBLE,
    KW_ELSE,
    KW_ENUM,
    KW_EXTERN,
    KW_FLOAT,
    KW_FOR,
    KW_GOTO,
    KW_IF,
    KW_INT,
    KW_LONG,
    KW_REGISTER,
    KW_RETURN,
    KW_SHORT,
    KW_SIGNED,
    KW_SIZEOF,
    KW_STATIC,
    KW_STRUCT,
    KW_SWITCH,
    KW_TYPEDEF,
    KW_UNION,
    KW_UNSIGNED,
    KW_VOID,
    KW_VOLATILE,
    KW_WHILE,
NUM_KEYWORDS
};
extern const char* strKeywords[NUM_KEYWORDS];

/**************************************************************************/
#define CASPIAN_MAX_ALIAS_SZ      CASPIAN_MAX_TOKEN_SZ
#define CASPIAN_MAX_ALIASED_TYPES 64
// #define CASPIAN_MAX_ALIASED_TYPES 2
#define CASPIAN_MAX_TYPE_CHARACTERISTICS 16

typedef struct {
    Token alias;
    Token characteristics[CASPIAN_MAX_TYPE_CHARACTERISTICS];
} TypeAlias;

extern TypeAlias gTypeAliases[CASPIAN_MAX_ALIASED_TYPES];
extern uint      gNumTypeAliases;
void addTypeAlias(const Token* alias, const Token characteristics[CASPIAN_MAX_TYPE_CHARACTERISTICS], const uint num_characteristics);
bool isolateTypeAlias(const SyntaxPtr sp, Token* alias);

bool isType(const Token* token);

#endif /* CASPIAN_KEYWORDS_H */