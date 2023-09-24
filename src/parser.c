#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

uint gTotalNodes = 0;
SyntaxPtr newSyntaxPtr(const Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], const uint num_tokens) {
    SyntaxPtr sp = (SyntaxPtr)malloc(sizeof(struct syntax_node_s));
    gTotalNodes++;
    
    sp->parent = NULL;
    
    sp->num_children = 0;
    for (uint i = 0; i<CASPIAN_MAX_SYNTAX_CHILDREN; i++)
        sp->children[i] = NULL;

    sp->num_tokens = num_tokens;
    copyTokens(sp->tokens, tokens, num_tokens);

    return sp;
}

static SyntaxPtr newMaster(const char* file_path) {
    const FileLine fl    = newFileLine(0, file_path, "#MASTER");
    const Token    token = newToken   (0, &fl      , "#MASTER");
    const Token    tokens[CASPIAN_MAX_TOKENS_IN_LINE] = {token};
    return newSyntaxPtr(tokens, 1);
}

uint gTotalFrees = 0;
void delSyntaxPtr(SyntaxPtr* sp) {
    if (*sp == NULL) return; // TODO: Realistically, this should never happen so it can probably be removed

    (*sp)->parent = NULL;
    for (uint i = 0; i<(*sp)->num_children; i++)
        delSyntaxPtr( &((*sp)->children[i]) );

    free(*sp); gTotalFrees++;
    *sp = NULL;
}

void printSyntaxPtr(const SyntaxPtr sp) {
    printf("(SyntaxPtr):\n");
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

void treeSyntaxPtr (const SyntaxPtr sp, const uint level) {
    for (uint i = 0; i<level; i++) printf("* ");
    if (sp->num_children>0) printf("(%u) ", sp->num_children);
    printTokens(sp->tokens, sp->num_tokens);

    for (uint c = 0; c<sp->num_children; c++)
        treeSyntaxPtr(sp->children[c], level+1);
}

static void addChild(SyntaxPtr parent, SyntaxPtr child) {
    if (parent->num_children >= (CASPIAN_MAX_SYNTAX_CHILDREN-1)) {
        warning_line(*(child->tokens[0].origin), "Too many lines in this scope! Line will be ignored...");
        // TODO: Safely exit here and clean-up all previous SyntaxPtrs? Or is a warning fine?
    }

    child->parent = parent;
    parent->children[parent->num_children] = child;
    parent->num_children++;
}

static inline bool isBlockCommentOpen (const Token* token) { return strcmp(token->text, "/*") == 0; }
static inline bool isBlockCommentClose(const Token* token) { return strcmp(token->text, "*/") == 0; }
static inline bool isBlockComment     (const Token* token) {
    return (
        isBlockCommentOpen(token)  ||
        isBlockCommentClose(token)
    );
}

static inline bool isUpScope(const Token* token) {
    return (
        (strcmp(token->text, "}") == 0) ||
        (strcmp(token->text, "]") == 0) ||
        (strcmp(token->text, ")") == 0) ||
        isBlockCommentClose(token)
    );
}
static inline bool isDownScope(const Token* token) {
    return (
        (strcmp(token->text, "{") == 0) ||
        (strcmp(token->text, "[") == 0) ||
        (strcmp(token->text, "(") == 0) ||
        isBlockCommentOpen (token)
    );
}
static inline bool isStatementEnd     (const Token* token) {
    return (
        (strcmp(token->text, ";") == 0) ||
        (strcmp(token->text, ",") == 0)
    );
}
static inline bool isScopeToken       (const Token* token) {
    return (
        isUpScope          (token) ||
        isDownScope        (token) ||
        isStatementEnd     (token)
    );
}

void splitAtScopeToken( Token original[CASPIAN_MAX_TOKENS_IN_LINE], uint* original_len,
                        Token    right[CASPIAN_MAX_TOKENS_IN_LINE], uint* right_len) {
    /*
        Splits a list of tokens at the first scope token
        into two left and right lists of tokens.
    */
    for (uint i = 0; i<(*original_len); i++) {
        if (isScopeToken( &(original[i]) )) {
            *right_len    = (*original_len)-i-1;               /* Slice scope token to end   */
            *original_len = i+1;                               /* Slice start to scope token */
            copyTokens(right, original + i + 1, (*right_len)); /* Copy right tokens into right array */
            return;
        }
    } *right_len = 0; /* If there was no scope token tell everyone else `right` isn't worth processing */
}

#include <assert.h>
SyntaxPtr buildSyntaxTree(const char* file_path, const FileLine file_as_lines[CASPIAN_MAX_LINES_IN_FILE], const uint num_file_lines) {
    /*
        Builds a syntax tree following the conventions of ANSI-C grammar.

        1) A `master` node is generated and a `current` node to track our
            position in the syntax tree.

        2) Lines are read from a list of FileLines which are then tokenized.

        3) These tokens are appended to a buffer (`token_buffer`).

        4) While the buffer contains a "scope" token ('{', '}', ';', or ','),
            a) The buffer is split in half at the first scope token -> `token_buffer` & `right`
            b) The left half is processed into a syntax node (`child`).
                i) This `child` node is added as a child to the `current` node.
                ii) Adjust the `current` node based on a criteria:
                    1) If the new `child` node opens a new scope (ends in '{'),
                        the `current` node moves "down" to the become the new `child`.
                    2) If the new `child` node closes the current scope (ends in '}'),
                        the `current` node moves "up" to its `parent` node.
                    3) If the new `child` node is a statement (ends in ';' or ','),
                        do nothing special.

        5) Once no more lines left to be read, append one final `child` node from
            any remaining tokens in the `token_buffer`.

        6) Return the `master` node, which points to the very top node in the syntax tree.
    */
    SyntaxPtr master  = newMaster(file_path); /* Head syntax node for the entire file */
    SyntaxPtr current = master;               /* Keeps track of the current node in the syntax tree */

    Token token_buffer[CASPIAN_MAX_TOKENS_IN_LINE];
    uint  token_buffer_len = 0;

    for (uint i = 0; i<num_file_lines; i++) {
        /* Read a line as tokens */
        Token tokens[CASPIAN_MAX_TOKENS_IN_LINE];
        const FileLine fl = file_as_lines[i];
        uint num_tokens   = tokenizeLine(&fl, tokens);
        if (num_tokens == 0) continue; /* Ignore blank lines with no tokens */

        /* Append the tokens to the buffer so we have a continuous stream */
        token_buffer_len = appendTokens(token_buffer, token_buffer_len, tokens, num_tokens);

        /* Split at scope up or down tokens */
        Token right[CASPIAN_MAX_TOKENS_IN_LINE];
        uint  right_len = 0;

        do { /* `do` loop because `right_len` needs to be set by `splitAtScopeToken` */
            splitAtScopeToken(token_buffer, &token_buffer_len, right, &right_len);

            if (right_len > 0) { /* If the right_len is non-zero, we had hit a scope token and should process the left group */
                const Token back_token  = backToken (token_buffer, token_buffer_len);
                const Token front_token = frontToken(token_buffer);
                
                if (
                    (token_buffer_len > 0)       &&  /* This should never happen anyhow */
                    !isBlockComment(&back_token) &&  /* Ignore all block comments       */
                    strcmp(front_token.text, ";")!=0 /* Ignore all stray semicolons because they don't affect the program */
                ) {
                    const uint child_len = token_buffer_len - (isStatementEnd(&back_token) ? 0 : 1);
                    if (child_len > 0) {
                        SyntaxPtr child = newSyntaxPtr(token_buffer, child_len);
                        addChild(current, child);
                    }

                    /* Now, start building the syntax tree */
                    if (  isUpScope(&back_token)) {
                        SyntaxPtr closer = newSyntaxPtr(token_buffer+token_buffer_len-1, 1);
                        addChild(current, closer); /* Add child BEFORE b/c the `closer` is a child of the current scope */
                        current = current->parent;
                    }
                    else if (isDownScope(&back_token)) {
                        SyntaxPtr opener = newSyntaxPtr(token_buffer+token_buffer_len-1, 1);
                        addChild(current, opener); /* Add child AFTER b/c the `opener` is a child of the previous line */
                        current = opener;
                    }
                }

                token_buffer_len = copyTokens(token_buffer, right, right_len); /* Shift the right into the left so we can repeat this process */
            }
        } while (right_len > 0);

    }
    /* Clean-up the tokens remaining in the buffer so they aren't forgotten */
    if (token_buffer_len > 0) {
        SyntaxPtr child = newSyntaxPtr(token_buffer, token_buffer_len);
        addChild(current, child);
    }

    return master;
}