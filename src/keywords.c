#include "keywords.h"

#include <string.h>

#include "utils.h"
#include "error.h"

const char* strKeywords[NUM_KEYWORDS] = {
    "auto",
    "break",
    "case",
    "char",
    "const",
    "continue",
    "default",
    "do",
    "double",
    "else",
    "enum",
    "extern",
    "float",
    "for",
    "goto",
    "if",
    "int",
    "long",
    "register",
    "return",
    "short",
    "signed",
    "sizeof",
    "static",
    "struct",
    "switch",
    "typedef",
    "union",
    "unsigned",
    "void",
    "volatile",
    "while"
};


/**************************************************************************/
/* Type Aliasing **********************************************************/

TypeAlias gTypeAliases[CASPIAN_MAX_ALIASED_TYPES];
uint gNumTypeAliases = 0;

#define NOT_TYPE_ALIAS -1
static int isTypeAlias(const Token* alias) {
    for (uint i = 0; i<gNumTypeAliases; i++) {
        if (cmpTokens(alias, &(gTypeAliases[i].alias))) return i;
    } return NOT_TYPE_ALIAS;
}

bool isPrimitiveType(const Token* token) {
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

    /* Check if it's a built-in type */
    if (isPrimitiveType(token)) return true;

    /* If not, try aliasing */
    bool is_type_alias = (isTypeAlias(token)!=NOT_TYPE_ALIAS);
    if (is_type_alias) return true;

    return false;
}

void addTypeAlias(const Token* alias, const Token characteristics[CASPIAN_MAX_TYPE_CHARACTERISTICS], const uint num_characteristics) {
    if (gNumTypeAliases>=CASPIAN_MAX_ALIASED_TYPES) {
        error(1, "Could not alias type " BMAGENTA "`%s`" RESET". Too many types aliased. Maximum is (%u)", alias->text, CASPIAN_MAX_ALIASED_TYPES);
    }

    TypeAlias new_type_alias;
    new_type_alias.alias = *alias;
    copyTokens(new_type_alias.characteristics, characteristics, num_characteristics);
    gTypeAliases[gNumTypeAliases++] = new_type_alias;
    printf(" ** Successfully added type alias " BMAGENTA "(%s)\n" RESET, alias->text);
}

bool isolateTypeAlias(const SyntaxPtr sp, Token* alias) {
    /*
        (9/24/23)
            JH - This routine sucked the life out of me for almost 2 hours.
            I'm sure there's a better way but I'm sleep deprived.

        Description:
            Identifies the alias within an ANSI-C styled typedef declaration.
            Supports pointer, function pointer, array types, structs, enums, and unions.
            Supports typedef from earlier typedefs. // FIXME: Confirm this???

        1) Travels through a tree of syntax nodes.
        2) Grabs the last token in its token list b/c this may be our alias name,
            but we don't know that yet.
        3) If it's a `{` then we're probably in a `typedef` + `struct`, `enum`, or `union`
            and we should ignore all the members inside it.
        4) If the syntax node has children, it might be worth investigating them for the alias name:
            a) Verify the first child isn't `[` because then we'd be at the end of a `typedef`
                and are defining an array bounds.
            b) If not, recurse this routine for each child in the syntax node tree,
                until we find the alias name. If we DON'T then we probably had it in that
                parent name we aliased at the start.
        5) Verify the alias name isn't a forbidden keyword, operator, delimiter, constant, etc.
        6) Verify the alias name isn't already used elsewhere.
        7) If all these tests pass -> You found your alias name.
    */

    *alias = backToken(sp->tokens, sp->num_tokens);
    if (cmpToken(alias, "{")) return false; /* Skip struct, enum, and union elements always */

    /* If this SyntaxPtr has children, then the alias is hidden in one of the children */
    if (sp->num_children > 0) {
        const SyntaxPtr first = sp->children[0];
        const Token     front = frontToken(first->tokens);
        if (cmpToken(&front, "[") == false) { /* Prevents skipping a valid parent for array typedefs */
            for (uint i = 0; i<sp->num_children; i++) {
                bool found = isolateTypeAlias(sp->children[i], alias);
                if (found) return true;
            }
        }
    }

    /* Otherwise, maybe it's hiding at the end of this one's tokens */
    if (cmpToken (alias, "*")) return false; /* Pointers aren't valid names */
    if (isInteger(alias))      return false; /* Numbers aren't valid names */
    if (
        cmpToken(alias, "(") || /* It's the start of a function pointer       */
        cmpToken(alias, ")") || /* It's the end of a function pointer         */
        cmpToken(alias, "[") || /* It's the start of an array size            */
        cmpToken(alias, "]") || /* It's the end of an array size              */
        cmpToken(alias, ",")    /* It's an enum member                        */
    ) return false;
    // TODO: Once enums and macros are implemented, implement if it tries to grab an or #define enum inside a [] array bound

    /* Alias is already in use or is a reserved keyword */
    if (isType(alias))
        error_token(1, *alias, "Alias type " BMAGENTA "(%s)" RESET " is already in use by reserved keyword or previous alias.", alias->text);

    return true;
}