// #include "ast_helpers.h"

// #include "error.h"
// #include "utils.h"

// static bool validCustomName(const char* name) {
//     const char* t = name;
//     if (isDigit(*t)) return false;
//     while (*t) {
//         if (
//             isDigit(*t) ||
//             isAlpha(*t) ||
//             (*t == '_')
//         ) t++;
//         else return false;
//     }
//     return true;
// }

// static bool withinFunctionHeader(AstPtr parent) {
//     AstPtr temp = parent;
//     while (temp != NULL) {
//         if (temp->op == AST_FUNCTION_HEADER) return true;
//         temp = temp->parent;
//     } return false;
// }

// AST_VERIFY(FunctionHeader) {
//     /*
//         Function Headers have a layout of the following:

//         ex:
//             static inline const char* func(const char* str)
//     */

//     enum HeaderLevel {
//         F_STATIC=0,
//         F_INLINE,
//         F_RETURN_TYPE,
//         F_FN_NAME,
//         F_OPEN_PAREN,
//         F_PARAMS,
//         F_CLOSE_PAREN,
//     NUM_HEADER_LEVEL_FLAGS
//     };
//     int header_level = F_STATIC;

//     // printf("\n----------------------\n");
//     // printSyntaxPtr(sp);

//     if (sp->num_children < 1)            return false; /* All function headers have one child, `(` */
//     if (withinFunctionHeader(prev_astp)) return false; /* If my parent (the prev AST node) was a header, we're parameters */

//     const Token front = frontToken(sp->tokens);
//     if (
//         cmpToken(&front, "typedef") ||
//         cmpToken(&front, "struct" ) ||
//         cmpToken(&front, "enum"   ) ||
//         cmpToken(&front, "union"  )
//     ) return false;

//     for (uint i = 0; i<sp->num_tokens; i++) {
        
//         const Token token = sp->tokens[i];
//         // printToken(&token);

//         if (cmpToken(&token, "static")) {
//             if (header_level > F_STATIC)
//                 error_token(1, token, "`static` must be first in function header.");
//             header_level++;
//         }

//         if (cmpToken(&token, "inline")) {
//             if (header_level > F_INLINE)
//                 error_token(1, token, "`inline` must be second in function header.");
//             header_level++;
//         }

//     }
//     const Token possible_fn_name = backToken(sp->tokens, sp->num_tokens);
//     if (!validCustomName(possible_fn_name.text)) {
//         error_token(1, possible_fn_name, "Invalid function name " BMAGENTA "`%s`" RESET, possible_fn_name.text);
//     }

//     assignIdentifier(child_astp, possible_fn_name);
//     return true;
// }

// AST_VERIFY(Typedef) {
//     return cmpToken(&frontToken(sp->tokens), "typedef");
// }