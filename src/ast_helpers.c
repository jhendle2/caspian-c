#include "ast_helpers.h"

#include "error.h"

AST_VERIFY(FunctionHeader) {
    /*
        Function Headers have a layout of the following:

        ex:
            static inline const char* func(const char* str)
    */

    enum HeaderLevel {
        F_STATIC=0,
        F_INLINE,
        F_RETURN_TYPE,
        F_FN_NAME,
        F_OPEN_PAREN,
        F_PARAMS,
        F_CLOSE_PAREN,
    NUM_HEADER_LEVEL_FLAGS
    };
    int header_level = F_STATIC;

    if (sp->num_children != 1)  return false; /* All function headers have one child, `(` */

    for (uint i = 0; i<sp->num_tokens; i++) {
        const Token token = sp->tokens[i];
        // printToken(&token);

        if (cmpToken(&token, "static")) {
            if (header_level > F_STATIC)
                error_token(1, token, "`static` must be first in function header.");
            header_level++;
        }

        if (cmpToken(&token, "inline")) {
            if (header_level > F_INLINE)
                error_token(1, token, "`inline` must be second in function header.");
            header_level++;
        }

        
    }

    return true;
}

AST_VERIFY(Typedef) {
    return cmpToken(&frontToken(sp->tokens), "typedef");
}