#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

#include "error.h"

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
TokenList popBackTokenList(TokenList head) {
    if (head == NULL) return NULL;
    if (head->next == NULL) {
        head->prev->next = NULL;
        head->prev = NULL;
        return head;
    } else return popBackTokenList(head->next);
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

TokenList pluckTokenList (TokenList item) {
    if (item->prev) item->prev->next = item->next;
    if (item->next) item->next->prev = item->prev;
    item->prev = NULL;
    item->next = NULL;
    return item;
}

void swapTokenList(TokenList a, TokenList b) {
    TokenList temp = newTokenList(&(b->token));
    b->token = a->token;
    a->token = temp->token;
    delTokenList(&temp);
}

TokenList popFrontTokenList(TokenList head) {
    swapTokenList(head, head->next);
    TokenList popped = head->next;
    head->next = head->next->next;
    head->next->prev = head;
    popped->prev = NULL;
    popped->next = NULL;
    return popped;
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

/***************************************************************************************/

static const char* strAstNodeType[] = {
    "InvalidNode",

    "Namespace",
    "TypeReference",
    "Identifier",

    "StatementEnd",
    "ParenExprBegin",
    "ListContinue",
    "ParenExprEnd",
    "BlockBegin",
    "BlockEnd",
    "IndexBegin",
    "IndexEnd",

    "FunctionModifier",
    "TypeModifier",

    "StructDecl",
    "UnionDecl",
    "EnumDecl",
    "AliasDecl",
    
    "IntegerConst",
    "FloatConst",
    "CharacterConst",
    "StringConst",

    "ReturnStatement",

    "FunctionDeclaration",
    "Function",
    "FunctionCall",

    "Operator",
    "AssignmentOperator",

    "VariableDeclaration",
    "VariableAssignment",
    "Expression",
};

AstPtr newAstPtr(const Token* token) {
    AstPtr astp = (AstPtr)malloc(sizeof(struct AstNode));

    astp->tokens = newTokenList(token);

    astp->parent   = NULL;
    astp->next     = NULL;
    astp->prev     = NULL;
    astp->children = NULL;
    // for (uint i = 0; i<CASPIAN_MAX_AST_CHILDREN; i++) {
    //     astp->children[i]  = NULL;
    // }
    // astp->num_children = 0;

    astp->node_type = InvalidNode;

    return astp;
}
void delAstPtr(AstPtr* astp) {
    if (*astp==NULL) return;

    (*astp)->parent = NULL;
    delTokenList(&((*astp)->tokens));

    // for (uint i = 0; i<(*astp)->num_children; i++)
    //     delAstPtr(&((*astp)->children[i]));
    (*astp)->prev = NULL;

    if ((*astp)->next) delAstPtr(&((*astp)->next));
    (*astp)->next = NULL;

    if ((*astp)->children) delAstPtr(&((*astp)->children));
    (*astp)->children = NULL;

    free(*astp);
    *astp = NULL;
}
uint lenAstPtr(AstPtr head) {
    if (head) return 1 + lenAstPtr(head->next);
    return 0;
}
void printAstPtr(const AstPtr astp) {
    // printf("{%s} <%u> [next=`%s`] [child=`%s`]",
    // printf("{%s} <%u> ",
    printf("{%s} ",
        strAstNodeType[astp->node_type]
        // , lenAstPtr(astp->children)
        // , astp->next ? astp->next->tokens->token.text : "NULL",
        // , astp->children ? astp->children->tokens->token.text : "NULL"
    ); printTokenList(astp->tokens); printf("\n");
}
void treeAstPtr(const AstPtr astp, const uint level) {
    for (uint i = 0; i<level; i++) printf("* ");
    printAstPtr(astp);

    AstPtr children = astp->children;
    if (children) treeAstPtr(children, level+1);

    AstPtr next = astp->next;
    if (next) treeAstPtr(next, level);
}

AstPtr getLast(AstPtr head) {
    // printf("get last\n");
    if (head->next) return getLast(head->next);
    return head;
}

static void addChild(AstPtr parent, AstPtr child) {
    // printf("adding (%s) to (%s) [%u]\n", child->tokens->token.text, parent->tokens->token.text, lenAstPtr(parent->children));
    child->parent = parent;
    if (parent->children) {
        AstPtr last = getLast(parent->children);
        last->next  = child;
        child->prev = last;
    } else {
        parent->children = child;
    }
}

static bool isOpenScopeToken(const Token* token) {
    return (
        cmpToken(token, "(") ||
        cmpToken(token, "[") ||
        cmpToken(token, "{")
    );
}
static bool isCloseScopeToken(const Token* token) {
    return (
        cmpToken(token, ")") ||
        cmpToken(token, "]") ||
        cmpToken(token, "}")
    );
}

static bool isScopePair(const Token* open, const Token* close) {
    if (cmpToken(open, "(") && cmpToken(close, ")")) return true;
    if (cmpToken(open, "[") && cmpToken(close, "]")) return true;
    if (cmpToken(open, "{") && cmpToken(close, "}")) return true;
    return false;
}

TokenList gTokenListMaster    =NULL,
          gTokenListStream    =NULL,
          gTokenListScopeStack=NULL,
          gTokenListLastPopped=NULL;
AstPtr    gAstMaster          =NULL;

AstPtr buildFirstPass(TokenList file_as_tokens) {
    gTokenListMaster = popFrontTokenList(file_as_tokens);
    gTokenListStream = file_as_tokens;
    TokenList current_token = file_as_tokens;

    TokenList scope_token_stack = newTokenList(&(gTokenListMaster->token));
    gTokenListScopeStack = scope_token_stack;

    gAstMaster = newAstPtr(&(gTokenListMaster->token));
    gAstMaster->node_type = Namespace;
    AstPtr current_astp = gAstMaster;

    while (current_token != NULL) {
        const Token token = current_token->token;

        AstPtr child_astp = newAstPtr(&token);
        addChild(current_astp, child_astp);

        if (isOpenScopeToken(&token)) {
            TokenList pushed = newTokenList(&token);
            pushBackTokenList(scope_token_stack, pushed);

            current_astp = child_astp;
        }
        else if (isCloseScopeToken(&token)) {
            gTokenListLastPopped = popBackTokenList(scope_token_stack);

            if (isScopePair(&(gTokenListLastPopped->token), &token)==tokenMismatch) {
                error_token_2(1, gTokenListLastPopped->token, token, "`%s` and `%s` are a mismatched pair.",
                    gTokenListLastPopped->token.text, token.text);
            }

            delTokenList(&gTokenListLastPopped);
            current_astp = current_astp->parent;
        }

        // printf("`%s`\n", current_token->token.text);
        current_token = current_token->next;
    }


    delTokenList(&gTokenListScopeStack);
    delTokenList(&gTokenListMaster);

    return gAstMaster;
}

#include "grammar.h"
AstPtr buildSecondPass(AstPtr first_pass_astp) {
    if (first_pass_astp == NULL) return NULL;
    printAstPtr(first_pass_astp);

    gAstMaster = first_pass_astp;
    AstPtr current_astp = gAstMaster;

    /* Discern my node type, combining any nodes that need to be */
    discernNodeType(&current_astp);

    /* And process anyone left over */
    buildSecondPass(current_astp->children);
    buildSecondPass(current_astp->next);
    current_astp = current_astp->next;

    return first_pass_astp;
}

AstPtr buildAstTree(TokenList file_as_tokens) {
    AstPtr first_pass_astp  = buildFirstPass(file_as_tokens);
    AstPtr second_pass_astp = buildSecondPass(first_pass_astp);
    return second_pass_astp;
}