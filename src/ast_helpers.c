#include "ast_helpers.h"

#include "error.h"

AST_VERIFY(FunctionHeader) {
    /*
        Function Headers have a layout of the following:

        ex:
            static inline const char* func(const char* str)
    */

    // bool flag_static      = false;
    // bool flag_inline      = false;
    // bool flag_return_type = false;
    // bool flag_name        = false;
    // bool flag_open_paren  = false;
    // bool flag_close_paren = false;

    // foreachToken(token, tokens, num_tokens) {
    //     printf("`%s` ", token.text);
    // } printf("\n");

    return true;
}

AST_VERIFY(Typedef) {
    return cmpToken(&frontToken(sp->tokens), "typedef");
}