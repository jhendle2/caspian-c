#include "types.h"

#include "error.h"

static uint gTypeMapIndex = 0;
TypeMapEntry gTypeMap[CASPIAN_MAX_CUSTOM_TYPES];

void addTypeAlias(const Token* key, const Token value[CASPIAN_MAX_TOKENS_IN_TYPE_RECALL], const uint value_len) {
    if (gTypeMapIndex == CASPIAN_MAX_CUSTOM_TYPES-1) {
        error_token(1, *key, "Cannot store any more custom types, maximum is (%u).", CASPIAN_MAX_CUSTOM_TYPES);
    }
    
    Token recalled[CASPIAN_MAX_TOKENS_IN_TYPE_RECALL]; uint recalled_len;
    if (recallTypeAlias(key, recalled, &recalled_len) == TYPE_KNOWN) {
        error_token(1, *key, "`%s` is already in use for: `%s & %u others...`  . Alias cannot be reassigned.", key->text, recalled[0].text, recalled_len)
    }

    gTypeMap[gTypeMapIndex].key       = *key;
    gTypeMap[gTypeMapIndex].value_len = value_len;
    copyTokens(gTypeMap[gTypeMapIndex].value, value, value_len);
    gTypeMapIndex++;
}
TypeStatus recallTypeAlias(const Token* key, Token value[CASPIAN_MAX_TOKENS_IN_TYPE_RECALL], uint* value_len) {
    for (uint i = 0; i<gTypeMapIndex; i++) {
        const TypeMapEntry map_entry = gTypeMap[i];
        if (cmpTokens( &(map_entry.key), key) ) {
            *value_len = map_entry.value_len;
            copyTokens(value, map_entry.value, *value_len);
            return TYPE_KNOWN;
        }
    }
    return TYPE_UNKNOWN;
}

static bool isPrimitiveType(const Token* token) {
    return (
        cmpToken(token, "char")   ||
        cmpToken(token, "double") ||
        cmpToken(token, "float")  ||
        cmpToken(token, "int")    ||
        cmpToken(token, "long")   ||
        cmpToken(token, "short")  ||
        cmpToken(token, "void")   // FIXME: This may cause problems in the future b/c `void` isn't really a type?
    );
}

bool isType(const Token* token) {
    Token values_to_be_discarded[CASPIAN_MAX_TOKENS_IN_TYPE_RECALL];
    uint  value_len_to_be_discarded;
    return (
        isPrimitiveType(token) ||
        (recallTypeAlias(token, values_to_be_discarded, &value_len_to_be_discarded) == TYPE_KNOWN)
    );
}