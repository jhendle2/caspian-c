#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

uint gTotalAstNodes = 0;
AstPtr newAstPtr(const Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], const uint num_tokens) {
    AstPtr sp = (AstPtr)malloc(sizeof(struct ast_node_s));
    gTotalAstNodes++;
    
    sp->parent = NULL;
    
    sp->num_children = 0;
    for (uint i = 0; i<CASPIAN_MAX_AST_CHILDREN; i++)
        sp->children[i] = NULL;

    sp->num_tokens = num_tokens;
    copyTokens(sp->tokens, tokens, num_tokens);

    return sp;
}

static AstPtr newMaster(const char* file_path) {
    const FileLine fl    = newFileLine(0, file_path, "#MASTER");
    const Token    token = newToken   (0, &fl      , "#MASTER");
    const Token    tokens[CASPIAN_MAX_TOKENS_IN_LINE] = {token};
    return newAstPtr(tokens, 1);
}

uint gTotalAstFrees = 0;
void delAstPtr(AstPtr* sp) {
    if (*sp == NULL) return; // TODO: Realistically, this should never happen so it can probably be removed

    (*sp)->parent = NULL;
    for (uint i = 0; i<(*sp)->num_children; i++)
        delAstPtr( &((*sp)->children[i]) );

    free(*sp); gTotalAstFrees++;
    *sp = NULL;
}

void printAstPtr(const AstPtr sp) {
    printf("(AstPtr):\n");
    printf("tokens        = "); printTokens(sp->tokens, sp->num_tokens);

    printf("parent        = ");
        if (sp->parent) printTokens(sp->parent->tokens, sp->parent->num_tokens);
        else printf("(none)\n");

    printf("children(%3u) = ", sp->num_children);
    if (sp->num_children==0) printf("(none)\n");
    else                     printf("\n");
    for (uint i = 0; i<sp->num_children; i++) {
        printf(" * "); printTokens(sp->children[i]->tokens, sp->children[i]->num_tokens);
    }

    printf("\n");
}

void treeAstPtr (const AstPtr sp, const uint level) {
    for (uint i = 0; i<level; i++) printf("* ");
    if (sp->num_children>0) printf("(%u) ", sp->num_children);
    printTokens(sp->tokens, sp->num_tokens);

    for (uint c = 0; c<sp->num_children; c++)
        treeAstPtr(sp->children[c], level+1);
}

static void addChild(AstPtr parent, AstPtr child) {
    child->parent = parent;
    parent->children[parent->num_children] = child;
    parent->num_children++;
}
