#include "parser2.h"

#include <stdlib.h>

#include "error.h"

SyntaxPtr gCurrentSyntaxMaster = NULL;
uint gTotalSyntaxNodes, gTotalSyntaxFrees;

SyntaxPtr newSyntaxPtr(const Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], const uint num_tokens) {
    SyntaxPtr sp = (SyntaxPtr)malloc(sizeof(struct SyntaxNode));
    gTotalSyntaxNodes++;

    sp->num_tokens = num_tokens;
    copyTokens(sp->tokens, tokens, num_tokens);

    sp->parent = NULL;

    sp->num_children = 0;
    for (uint i = 0; i<CASPIAN_MAX_SYNTAX_CHILDREN; i++)
        sp->children[i] = NULL;
        
    return sp;
}

// #include <assert.h>
void delSyntaxPtr(SyntaxPtr* sp) {
    if (!sp || !*sp) return;

    (*sp)->parent = NULL;
    for (uint i = 0; i<(*sp)->num_children; i++)
        delSyntaxPtr( &(*sp)->children[i] );

    free(*sp);
    gTotalSyntaxFrees++;
    *sp = NULL;
}


void printSyntaxPtr(const SyntaxPtr sp) {
    printf("\n(SyntaxPtr):\n");
    printf("Parent: ");
        if (sp->parent) printTokens(sp->parent->tokens, sp->parent->num_tokens);
        else            printf("(null)\n");
    printf("Tokens: ");
        printTokens(sp->tokens, sp->num_tokens);
    printf("Children (%u): ", sp->num_children);
    if (sp->num_children) {
        for (uint i = 0; i<sp->num_children; i++) {
            printf("(%u) ", i); printTokens(sp->children[i]->tokens, sp->children[i]->num_tokens);
        }
    } else printf("(none)\n");
    printf("(EndSyntaxPtr)\n");
}
void treeSyntaxPtr (const SyntaxPtr sp, const uint level) {
    for (uint i = 0; i<level+1; i++) printf("* ");
    printTokens(sp->tokens, sp->num_tokens);
    for (uint i = 0; i<sp->num_children; i++) {
        treeSyntaxPtr(sp->children[i], level+1);
    }
}

static SyntaxPtr newMaster(const char* file_path) {
    generateMasterTokens(file_path, MASTER_TOKENS);
    return newSyntaxPtr(MASTER_TOKENS, 1);
}
static void addChild(SyntaxPtr parent, SyntaxPtr child) {
    if (parent->num_children >= (CASPIAN_MAX_SYNTAX_CHILDREN-1)) {
        warning_line(child->tokens[0].origin, "Too many lines in this scope! Line will be ignored...");
        // TODO: Safely exit here and clean-up all previous SyntaxPtrs? Or is a warning fine?
    }

    child->parent = parent;
    parent->children[parent->num_children] = child;
    parent->num_children++;
}

static inline bool isDownScope(const Token* token_ptr) {
    return (
        cmpToken(token_ptr, "/*") ||
        cmpToken(token_ptr, "{")  ||
        cmpToken(token_ptr, "[")  ||
        cmpToken(token_ptr, "(")  ||
        
        cmpToken(token_ptr, "typedef")  ||
        cmpToken(token_ptr, "struct")  ||
        cmpToken(token_ptr, "union")  ||
        cmpToken(token_ptr, "enum")
    );
}
static inline bool isUpScope(const Token* token_ptr) {
    return (
        cmpToken(token_ptr, "*/") ||
        cmpToken(token_ptr, "}")  ||
        cmpToken(token_ptr, "]")  ||
        cmpToken(token_ptr, ")")
    );
}
static inline bool isScopeToken(const Token* token_ptr) {
    return (
        cmpToken(token_ptr, ";") ||
        cmpToken(token_ptr, ",") ||
        isDownScope(token_ptr)   ||
        isUpScope(token_ptr)
    );
}

static bool splitAtScopeFirstToken(
        Token  tokens[CASPIAN_MAX_TOKENS_IN_LINE], const uint  tokens_len,
        Token* scope_token,
        Token  left  [CASPIAN_MAX_TOKENS_IN_LINE],       uint* left_len,
        Token  right [CASPIAN_MAX_TOKENS_IN_LINE],       uint* right_len
) {
    for (uint i = 0; i<tokens_len; i++) {
        const Token current_token = tokens[i];
        // printf("@(%s)\n", current_token.text);
        if (isScopeToken(&current_token)) {
            
            *left_len  = i;
            *right_len = tokens_len-i-1;

            *scope_token = current_token;
            moveTokens(left , tokens    , *left_len );
            moveTokens(right, tokens+(*left_len)+1, *right_len);
            
            return true;
        }
    }
    *left_len  = 0;
    *right_len = 0;
    return false;
}

static bool isMatchingScopeToken(const Token* opener, const Token* closer) {
    return (
        (cmpToken(opener, "{") && cmpToken(closer, "}")) ||
        (cmpToken(opener, "[") && cmpToken(closer, "]")) ||
        (cmpToken(opener, "(") && cmpToken(closer, ")"))
    );
}

SyntaxPtr buildSyntaxTree(const char* file_path, const FileLine file_as_lines[CASPIAN_MAX_LINES_IN_FILE], const uint num_file_lines) {
    SyntaxPtr master  = newMaster(file_path);
    SyntaxPtr current = master;

    Token token_buffer[CASPIAN_MAX_TOKENS_IN_LINE];
    uint num_tokens_in_buffer = 0;

    Token last_block_comment_opener;
    Token last_scope_opener_stack[CASPIAN_MAX_TOKENS_IN_LINE];
    uint  last_scope_opener_stack_index = 0;

    #define backScopeStack() last_scope_opener_stack[last_scope_opener_stack_index-1]

    #define pushToScopeStack(TOKEN) \
        if (last_scope_opener_stack_index == CASPIAN_MAX_SYNTAX_DEPTH-1) {\
            error_token(1, TOKEN, "Maximum scope depth has been reached. Please try and move these blocks into functions");\
        }\
        last_scope_opener_stack[last_scope_opener_stack_index++] = TOKEN;

    #define popFromScopeStack() \
        if (last_scope_opener_stack_index == 0) {\
            error_token(1, last_scope_opener_stack[0], "Scope token mismatch. This should never happen!");\
        } last_scope_opener_stack_index--;

    bool in_block_comment = false;
    for (uint i = 0; i<num_file_lines; i++) {
        const FileLine line_read = file_as_lines[i];
        Token tokens_read[CASPIAN_MAX_TOKENS_IN_LINE];
        const uint num_tokens_read = tokenizeLine(&line_read, tokens_read);

        num_tokens_in_buffer = appendTokens(token_buffer, num_tokens_in_buffer, tokens_read, num_tokens_read);
        // if (num_tokens_in_buffer) { printf("\ntoken_buffer="); printTokens(token_buffer, num_tokens_in_buffer); }

        uint left_len=0, right_len=0;
        Token left[CASPIAN_MAX_TOKENS_IN_LINE], right[CASPIAN_MAX_TOKENS_IN_LINE];
        Token scope_token;

        while (
            splitAtScopeFirstToken(
                token_buffer, num_tokens_in_buffer,
                &scope_token,
                left , &left_len,
                right, &right_len
            )
        ) {
            // printf("left=");        printTokens(left, left_len);
            // printf("scope_token="); printTokens(&scope_token, 1);
            // printf("right=");       printTokens(right, right_len);
            // printf("\n");

            const bool in_block_comment_opener = cmpToken(&scope_token, "/*");
            const bool in_block_comment_closer = cmpToken(&scope_token, "*/");

            if (in_block_comment_opener) {
                last_block_comment_opener = scope_token;
                if (in_block_comment) {
                    error_token_2(1, last_block_comment_opener, scope_token, "Nested block comments are invalid");
                }
                in_block_comment = true;
                num_tokens_in_buffer = moveTokens(token_buffer, right, right_len);
            }
            if (in_block_comment_closer) {
                if (!in_block_comment) {
                    error_token(1, scope_token, "Attempted to close a block comment without previously opening one. Did you forget a `*/`?");
                }
                num_tokens_in_buffer = moveTokens(token_buffer, right, right_len);
                left_len = 0;
                in_block_comment = false;
            }

            if (!in_block_comment && !in_block_comment_closer) {

                const bool is_stmt = cmpToken(&scope_token, ";") || cmpToken(&scope_token, ",");
                SyntaxPtr scope_ptr = (is_stmt)
                                        ? newSyntaxPtr(left, /*left_len) */ pushBackTokens(left, left_len, &scope_token))
                                        : newSyntaxPtr(&scope_token, 1);
                
                if (!is_stmt && left_len > 0) {
                    SyntaxPtr left_ptr = newSyntaxPtr(left, left_len);
                    addChild(current, left_ptr);
                }
                addChild(current, scope_ptr);
                
                if (isUpScope(&scope_token)) {
                    if (!isMatchingScopeToken(&backScopeStack(), &scope_token)) {
                        error_token_2(1, backScopeStack(), scope_token,
                            "The scope tokens `%s` and `%s` are a mismatch. Did you forget to close `%s`?",
                            backScopeStack().text, scope_token.text, backScopeStack().text);
                    }
                    popFromScopeStack();
                    current = current->parent;
                }
                else if (isDownScope(&scope_token)) {
                    pushToScopeStack(scope_token);
                    // printf("[SETTING LAST-SCOPE-OPENER `%s`]\n", backScopeStack().text);
                    current = scope_ptr;
                }

                // printf("\n");
            }
            num_tokens_in_buffer = moveTokens(token_buffer, right, right_len);
        }
    }
    if (in_block_comment) {
        error_token(1, last_block_comment_opener, "This block comment was never properly closed");
    }
    if (current != master) {
        error_token(1, backScopeStack(), "This scope was never properly closed");
    }

    return master;
}