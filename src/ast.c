#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

TokenList newTokenList(const Token* token) {
    TokenList alist = (TokenList)malloc(sizeof(struct token_list_s));
    alist->token    = *token;
    alist->prev     = NULL; alist->next = NULL;
    return alist;
}
void pushBackTokenList(TokenList head, TokenList item) {
    if (head->next == NULL) {
        head->next = item;
        item->prev = head;
    } else pushBackTokenList(head->next, item);
}
void delTokenList(TokenList* head) {
    if (*head==NULL) return;

    (*head)->prev = NULL;
    if ((*head)->next) delTokenList(&((*head)->next));
    (*head)->next = NULL;

    free(*head);
    *head = NULL;
}
void printTokenList(TokenList head) {
    printf("`%s` ", head->token.text);
    if (head->next) printTokenList(head->next);
}



TokenList buildTokenListFromLines(const char* file_path, const FileLine file_as_lines[CASPIAN_MAX_LINES_IN_FILE], const uint num_file_lines) {
    /***************************************************************************************/
    /* This allows us to read tokens as if they were coming one by one from a stream */
    uint current_line_index = 0;
    Token current_token;
    Token tokens_read[CASPIAN_MAX_TOKENS_IN_LINE];
    uint num_tokens_read = 0;
    uint tokens_read_index = 0;
    bool tokens_remaining = true;

    #define nextToken(TOKEN) {\
        if (tokens_read_index == num_tokens_read) { /* Load up a new batch of tokens */ \
            if (current_line_index == num_file_lines) {\
                tokens_remaining = false;\
            } else {\
                num_tokens_read = 0;\
                while (num_tokens_read == 0 && current_line_index < num_file_lines) { /* Must check if more lines left */ \
                    tokens_read_index = 0;\
                    num_tokens_read = tokenizeLine( &(file_as_lines[current_line_index++]), tokens_read );\
                }\
                if (num_tokens_read == 0) tokens_remaining = false; \
            }\
        }\
        if (tokens_remaining) current_token = tokens_read[tokens_read_index++];\
    }
    #define startTokenStream(TOKEN) {\
        current_line_index = 0;\
        tokens_remaining   = true;\
        nextToken(TOKEN); /* Loads up the first token */ \
    }
    
    /***************************************************************************************/
    /* Create a linked-list of tokens nodes for each token in the file */
    generateMasterToken(file_path, MASTER_TOKENS);
    TokenList file_as_tokens = newTokenList(MASTER_TOKENS);

    startTokenStream(current_token);
    do {
        /* Once we see a block comment begin, keep going until we see its end */
        if (cmpToken(&current_token, "/*")) {
            while (tokens_remaining) {
                nextToken();
                if (cmpToken(&current_token, "*/")) break;
            }
            nextToken(); /* Same condition as end of do-while b/c we want to continue here */
            continue;
        }

        /* Otherwise, append the token and grab the next */
        TokenList new_token = newTokenList(&current_token);
        pushBackTokenList(file_as_tokens, new_token);

        nextToken(current_token);
    } while (tokens_remaining);

    return file_as_tokens;
}