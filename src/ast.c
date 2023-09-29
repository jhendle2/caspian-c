// #include "ast.h"

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// #include "error.h"
// #include "ast_helpers.h"
// #include "keywords.h"

// static const char* strAstOp[NUM_AST_OPS] = {
//     "INVALID",
//     "MASTER",
//     "NAMESPACE",

//     "VAR_INIT",

//     "FOR_LOOP",
//     "WHILE_LOOP",
        
//     "CONTINUE",
//     "BREAK",
//     "GOTO",

//     "IF",
//     "ELSE",

//     "STRUCT",
//     "ENUM",
//     "UNION",

//     "TYPE_ALIAS",

//     "FUNCTION_HEADER",
//     "FUNCTION_CALL",
//     "RETURN",

//     "INTEGER_CONSTANT",
//     "STRING_CONSTANT",

//     "CAST",
// };

// uint gTotalAstNodes = 0;
// AstPtr newAstPtr  (const enum AstOp op, const Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], const uint num_tokens) {
//     AstPtr astp = (AstPtr)malloc(sizeof(struct ast_node_s));
//     gTotalAstNodes++;
    
//     astp->has_identifier = false;

//     astp->op = op;
//     astp->parent = NULL;
    
//     astp->num_children = 0;
//     for (uint i = 0; i<CASPIAN_MAX_AST_CHILDREN; i++)
//         astp->children[i] = NULL;

//     astp->num_tokens = num_tokens;
//     copyTokens(astp->tokens, tokens, num_tokens);

//     return astp;
// }

// static AstPtr newMaster(const char* file_path) {
//     generateMasterTokens(file_path, MASTER_TOKENS);
//     return newAstPtr(AST_MASTER, MASTER_TOKENS, 1);
// }

// uint gTotalAstFrees = 0;
// void delAstPtr(AstPtr* astp) {
//     if (*astp == NULL) return; // TODO: Realistically, this should never happen so it can probably be removed

//     (*astp)->parent = NULL;
//     for (uint i = 0; i<(*astp)->num_children; i++)
//         delAstPtr( &((*astp)->children[i]) );

//     free(*astp); gTotalAstFrees++;
//     *astp = NULL;
// }

// void assignIdentifier(AstPtr astp, const Token identifier) {
//     astp->identifier     = identifier;
//     astp->has_identifier = true;
// }

// void printAstPtr(const AstPtr astp) {
//     // printf("(AstPtr):\n");
//     // printf("tokens        = "); printTokens(astp->tokens, astp->num_tokens);

//     // printf("parent        = ");
//     //     if (astp->parent) printTokens(astp->parent->tokens, astp->parent->num_tokens);
//     //     else printf("(none)\n");

//     // printf("children(%3u) = ", astp->num_children);
//     // if (astp->num_children==0) printf("(none)\n");
//     // else                     printf("\n");
//     // for (uint i = 0; i<astp->num_children; i++) {
//     //     printf(" * "); printTokens(astp->children[i]->tokens, astp->children[i]->num_tokens);
//     // }

//     // printf("\n");
// }

// void treeAstPtr (const AstPtr astp, const uint level) {
//     for (uint i = 0; i<level; i++) printf("* ");
//     if (astp->num_children>0) printf("(%u) ", astp->num_children);
//     if (astp->has_identifier) printf("AST(%s) [%s]: ", strAstOp[astp->op], astp->identifier.text);
//     else printf("AST(%s): ", strAstOp[astp->op]);
//     printTokens(astp->tokens, astp->num_tokens);

//     for (uint c = 0; c<astp->num_children; c++)
//         treeAstPtr(astp->children[c], level+1);
// }

// static void addChild(AstPtr parent, AstPtr child) {
//     child->parent = parent;
//     parent->children[parent->num_children] = child;
//     parent->num_children++;
// }

// /****************************************************************************************************/
// /* AST Building *************************************************************************************/

// AstPtr gCurrentAstMaster = NULL;

// AstPtr buildAstTreeHelper(AstPtr prev_astp, const SyntaxPtr current_sp) {
//     /* Aliases for ease */
//     // printf(" -- "); printTokens(current_sp->tokens, current_sp->num_tokens);
//     // printf("is Typedef? %s\n", cmpToken(current_sp->tokens+0,"typedef")?"YES":"NO");
//     const Token first = frontToken(current_sp->tokens);
//     AstPtr child_astp = newAstPtr(AST_INVALID, current_sp->tokens, current_sp->num_tokens);

//     if (cmpToken(&first, MASTER_TOKEN_STR)) {
//         // printf("[1]\n");
//         child_astp->op = AST_MASTER;
//         return child_astp;
//     }

//     if (AST_VERIFY_FunctionHeader(prev_astp, child_astp, current_sp)) {
//         // printf("[2]\n");
//         child_astp->op = AST_FUNCTION_HEADER;
//         return child_astp;
//     }

//     if (AST_VERIFY_Typedef(prev_astp, child_astp, current_sp)) {
//         // printf("[3]\n");
//         Token alias;
//         child_astp->op = AST_TYPE_ALIAS;
//         bool found = isolateTypeAlias(current_sp, &alias);
//         if (found) addTypeAlias(&alias, current_sp->tokens, current_sp->num_tokens);
//     }

//     return child_astp;
// }

// AstPtr buildAstTreeRecursor(AstPtr current_astp, const SyntaxPtr current_sp) {
//     AstPtr child_astp = buildAstTreeHelper(current_astp, current_sp);
//     addChild(current_astp, child_astp);
//     // AstPtr prev_astp = current_astp;
//     current_astp = child_astp;

//     for (uint i = 0; i<current_sp->num_children; i++) {
//         const SyntaxPtr child = current_sp->children[i];
//         buildAstTreeRecursor(current_astp, child);
//     }

//     // current_astp = prev_astp;
//     return current_astp;
// }

// AstPtr buildAstTree(const char* file_path) {
//     SyntaxPtr master_sp = buildSyntaxTree(file_path);
//     masterTreeSyntaxPtr(master_sp); printf("\n"); // TODO: Remove

//     AstPtr master_astp  = newMaster(file_path);
//     AstPtr current_astp = master_astp;
//     gCurrentAstMaster   = master_astp;

//     buildAstTreeRecursor(current_astp, master_sp);
//     masterTreeAstPtr(current_astp); printf("\n"); // TODO: Remove
    
//     delSyntaxPtr(&master_sp);
//     return master_astp;
// }