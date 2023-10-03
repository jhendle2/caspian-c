#ifndef CASPIAN_TYPES_H
#define CASPIAN_TYPES_H

#include "lexer.h"

#define CASPIAN_MAX_CUSTOM_TYPES 128
#define CASPIAN_MAX_TOKENS_IN_TYPE_RECALL 16

typedef struct {
    uint value_len;
    Token key, value[CASPIAN_MAX_TOKENS_IN_TYPE_RECALL];
} TypeMapEntry;
extern TypeMapEntry gTypeMap[CASPIAN_MAX_CUSTOM_TYPES];

void  addTypeAlias   (const Token* key, const Token value[CASPIAN_MAX_TOKENS_IN_TYPE_RECALL], const uint value_len);

enum TypeStatus {
    TYPE_UNKNOWN,
    TYPE_KNOWN
};
enum TypeStatus recallTypeAlias(const Token* key, Token value[CASPIAN_MAX_TOKENS_IN_TYPE_RECALL], uint* value_len);

bool isType(const Token* token);

#endif /* CASPIAN_TYPES_H */