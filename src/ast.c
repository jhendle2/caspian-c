#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "types.h"
#include "constants.h"

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

AstPtr gCurrentAstMaster = NULL;
uint gTotalAstNodes, gTotalAstFrees;

AstPtr newAstPtr(const Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], const uint num_tokens) {
    AstPtr astp = (AstPtr)malloc(sizeof(struct AstNode));
    if (astp == NULL) error_token(1, tokens[0], "System ran out of memory and no new AST nodes could be allocated");

    gTotalAstNodes++;

    astp->node_type = InvalidNode;

    astp->num_tokens = num_tokens;
    copyTokens(astp->tokens, tokens, num_tokens);

    astp->parent = NULL;
    astp->child  = NULL;
    astp->prev   = NULL;
    astp->next   = NULL;

    return astp;
}
void delAstPtr(AstPtr* astp) {
    if (!astp || !*astp) return;

    (*astp)->parent = NULL;
    (*astp)->prev   = NULL;

    AstPtr child = (*astp)->child;
    if (child) delAstPtr(&child);
    (*astp)->child = NULL;

    AstPtr next = (*astp)->next;
    if (next) delAstPtr(&next);
    (*astp)->next = NULL;

    free(*astp);
    gTotalAstFrees++;
    *astp = NULL;
}

void printAstPtr(const AstPtr astp) {
    if (astp==NULL) printf("(null) ");
    
    if (astp->num_tokens > 0) {
        printf("{%-15s: ", strAstNodeType[astp->node_type]);
        printTokensNoNewline(astp->tokens, astp->num_tokens);
        printf("}\n");
    } else {
        printf("[[ %-15s ]]\n", strAstNodeType[astp->node_type]);
    }
}

// void dumpAstPtr(const AstPtr astp) {
//     printf("\n(AstPtr):\n");
//     // printf("Type: %s\n", strAstOp[astp->op]);
//     printf("Parent: ");
//         if (astp->parent) printTokens(astp->parent->tokens, astp->parent->num_tokens);
//         else            printf("(null)\n");
//     printf("Tokens: ");
//         printTokens(astp->tokens, astp->num_tokens);
//     // printf("Children (%u): ", astp->num_children);
//     // if (astp->num_children) {
//     //     for (uint i = 0; i<astp->num_children; i++) {
//     //         printf("(%u) ", i); printTokens(astp->children[i]->tokens, astp->children[i]->num_tokens);
//     //     }
//     // } else printf("(none)\n");
//     printf("(EndAstpPtr)\n");
// }

void listAstPtr(const AstPtr head) {
    printf("- "); printAstPtr(head);
    if (head->next) listAstPtr(head->next);
}

void treeAstPtr(const AstPtr astp, const uint level) {
    for (uint i = 0; i<level+1; i++) printf("- ");
    printAstPtr(astp);
    if (astp->child) {
        AstPtr child = astp->child;
        while (child) {
            treeAstPtr(child, level+1);
            child = child->next;
        }
    }
}

#include <assert.h>
void appendAstPtr(AstPtr astp, AstPtr next) {
    AstPtr last = astp;
    while (
        last->next != NULL &&
        last->next != last /* FIXME: This resolves a weird bug where the loop continues forever but it's a hack! */
    ) {
        // printf("HERE(%s)\n", last->tokens[0].text);
        last = last->next;
    }
    last->next = next;
    next->prev = last;
}

static void addChild(AstPtr parent, AstPtr child) {
    child->parent = parent;

    if (parent->child == NULL) {
        parent->child = child;
    }
    else {
        appendAstPtr(parent->child, child);
    }
}

/***************************************************************************************/

static AstNodeType determineNodeType(const Token* token) {
    if (isType        (token))      return TypeReference;
    if (isIntegerConst(token))      return IntegerConst;
    if (isCharConst   (token))      return CharacterConst;
    if (isStringConst (token))      return StringConst;
    if (isFloatConst  (token))      return FloatConst;

    if (cmpToken(token, ";"))       return StatementEnd;
    if (cmpToken(token, "("))       return ParenExprBegin;
    if (cmpToken(token, ","))       return ListContinue;
    if (cmpToken(token, ")"))       return ParenExprEnd;
    if (cmpToken(token, "["))       return IndexBegin;
    if (cmpToken(token, "]"))       return IndexEnd;
    if (cmpToken(token, "{"))       return BlockBegin;
    if (cmpToken(token, "}"))       return BlockEnd;
    if (cmpToken(token, "="))       return AssignmentOperator;
    if (isOperatorDelimiter(token)) return Operator;

    if (cmpToken(token, "static"))  return FunctionModifier;
    if (cmpToken(token, "inline"))  return FunctionModifier;
    if (cmpToken(token, "const"))   return TypeModifier;
    if (cmpToken(token, "return"))  return ReturnStatement;
    if (cmpToken(token, "struct"))  return StructDecl;
    if (cmpToken(token, "enum"))    return EnumDecl;
    if (cmpToken(token, "union"))   return UnionDecl;
    if (cmpToken(token, "typedef")) return AliasDecl;

    return Identifier;
}

static AstPtr copyAstPtr(const AstPtr original) {
    /*
        Does not copy relationships between this node and other nodes
        because WILL lead to double frees later.
    */
    AstPtr new_astp = newAstPtr(original->tokens, original->num_tokens);
    new_astp->node_type = original->node_type;
    return new_astp;
}

static void swapChildren(AstPtr left, AstPtr right) {
    /*
        If you absolutely need to transfer children between two nodes,
        you must swap them, not copy them. Otherwise, double frees will occur.
    */
    left->child  = right->child;
    right->child = NULL;
}

static AstPtr buildFirstPassTree(const char* file_path, const FileLine file_as_lines[CASPIAN_MAX_LINES_IN_FILE], const uint num_file_lines) {
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
        nextToken(TOKEN);\
    }
    
    /***************************************************************************************/

    /* Create a linked-list of AST nodes for each token in the file */
    generateMasterTokens(file_path, MASTER_TOKENS);
    AstPtr first_pass_tree  = newAstPtr(MASTER_TOKENS, 1);
    AstPtr current_astp = first_pass_tree;
    gCurrentAstMaster      = first_pass_tree;

    first_pass_tree->node_type = Namespace;

    // uint debug_depth = 0;

    AstPtr prev_astp = first_pass_tree;
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
        // printf("@(%s)\n", current_token.text);

        enum AstNodeType node_type = determineNodeType(&current_token);

        /* Aggregate tokens which should be combined */
        if (cmpToken(&current_token, "*") && prev_astp->node_type == TypeReference) {
            prev_astp->num_tokens = pushBackTokens(prev_astp->tokens, prev_astp->num_tokens, &current_token);
            nextToken();
            continue;
        }
        if (node_type == FunctionModifier && prev_astp->node_type == FunctionModifier) {
            prev_astp->num_tokens = pushBackTokens(prev_astp->tokens, prev_astp->num_tokens, &current_token);
            nextToken();
            continue;
        }
        if (node_type == TypeReference && prev_astp->node_type == TypeModifier) {
            prev_astp->num_tokens = pushBackTokens(prev_astp->tokens, prev_astp->num_tokens, &current_token);
            prev_astp->node_type  = TypeReference;
            nextToken();
            continue;
        }

        AstPtr child_astp = newAstPtr(&current_token, 1);
        child_astp->node_type = node_type;
        addChild(current_astp, child_astp);

        const Token first = child_astp->tokens[0];
        if (
            cmpToken(&first, "(") ||
            cmpToken(&first, "{") ||
            cmpToken(&first, "[")
        ) {
            // printf("[GOING DOWN %u]\n", ++debug_depth);
            current_astp = child_astp;
        }

        if (
            cmpToken(&first, ")") ||
            cmpToken(&first, "}") ||
            cmpToken(&first, "]")
        ) {
            // printf("[GOING UP %u]\n", --debug_depth);
            current_astp = current_astp->parent;
            if (current_astp == NULL) {
                printf("[CRITICAL SCOPE ERROR!]\n");
                exit(1); // TODO: Figure out when this happens and if it ever should occur for real (or if it's a compiler bug)
            }
        }

        prev_astp = child_astp;
        nextToken(current_token);
    } while (tokens_remaining);

    return first_pass_tree;
}

#define next1(NODE) (NODE)->next
#define next2(NODE) (NODE)->next->next
#define next3(NODE) (NODE)->next->next->next
#define next4(NODE) (NODE)->next->next->next->next

#define cmpNext1(NODE, CASE) ( next1(NODE) && next1(NODE)->node_type == CASE )
#define cmpNext2(NODE, CASE) ( next2(NODE) && next2(NODE)->node_type == CASE )
#define cmpNext3(NODE, CASE) ( next3(NODE) && next3(NODE)->node_type == CASE )
#define cmpNext4(NODE, CASE) ( next4(NODE) && next4(NODE)->node_type == CASE )

static AstPtr processAstNode(AstPtr* node_to_process) {
    /*
        Determines syntax of a stream of tokens.
        May need additional tokens to the right of a given token.
        If these additional tokens are consumed, the stream is shifted
        past those tokens so they are not double counted.
    */

    AstPtr processed_node = newAstPtr((Token*){}, 0);
    AstPtr copy_node = copyAstPtr(*node_to_process);

    const enum AstNodeType node_type = (*node_to_process)->node_type;

    if (node_type ==  FunctionModifier) { /* static, inline ... */
        addChild(processed_node, copy_node);
        if ( /* MODIFIER TYPE IDENTIFIER ( */
            cmpNext1(*node_to_process, TypeReference ) &&
            cmpNext2(*node_to_process, Identifier    ) &&
            cmpNext3(*node_to_process, ParenExprBegin)
        ) {
            addChild(processed_node, copyAstPtr(next1(*node_to_process)));
            addChild(processed_node, copyAstPtr(next2(*node_to_process)));

            /* Arguments get dropped otherwise */
            AstPtr arguments = next3(*node_to_process);
            AstPtr arguments_copy = copyAstPtr(arguments);
            swapChildren(arguments_copy, arguments);
            addChild(processed_node, arguments_copy);
            // addChild(processed_node, copyAstPtr(next3(*node_to_process)));
            
            if (cmpNext4(*node_to_process, BlockBegin)) { /* MODIFIER TYPE IDENTIFIER ( { */
                addChild(processed_node, copyAstPtr(next4(*node_to_process)));
                processed_node->node_type = Function;
                (*node_to_process) = next4(*node_to_process);
            }
            else if (cmpNext4(*node_to_process, StatementEnd)) { /* MODIFIER TYPE IDENTIFIER ( ; */
                addChild(processed_node, copyAstPtr(next4(*node_to_process)));
                processed_node->node_type = FunctionDeclaration;
                (*node_to_process) = next4(*node_to_process);
            }
        }
    }
        
    if (node_type == TypeReference) { /* TYPE */
        if ( /* TYPE IDENTIFIER */
            cmpNext1(*node_to_process, Identifier )
        ) {
            addChild(processed_node, copy_node);
            addChild(processed_node, copyAstPtr(next1(*node_to_process)));

            if ( /* TYPE IDENTIFIER ( */
                cmpNext2(*node_to_process, ParenExprBegin)
            ) {
                /* Arguments get dropped otherwise */
                AstPtr arguments = next2(*node_to_process);
                AstPtr arguments_copy = copyAstPtr(arguments);
                swapChildren(arguments_copy, arguments);
                addChild(processed_node, arguments_copy);
                // addChild(processed_node, copyAstPtr(next2(*node_to_process)));
                
                if (cmpNext3(*node_to_process, BlockBegin)) { /* TYPE IDENTIFIER ( { */
                    addChild(processed_node, copyAstPtr(next3(*node_to_process)));
                    processed_node->node_type = Function;
                    (*node_to_process) = next3(*node_to_process);
                }
                else if (cmpNext3(*node_to_process, StatementEnd)) { /* TYPE IDENTIFIER ( ; */
                    addChild(processed_node, copyAstPtr(next3(*node_to_process)));
                    processed_node->node_type = FunctionDeclaration;
                    (*node_to_process) = next3(*node_to_process);
                }
            }

            else if ( /* TYPE IDENTIFIER ; */
                cmpNext2(*node_to_process, StatementEnd)
            ) {
                addChild(processed_node, copyAstPtr(next2(*node_to_process)));
                processed_node->node_type = VariableDeclaration;
                (*node_to_process) = next2(*node_to_process);
            }

            else if ( /* TYPE IDENTIFIER = ... ; */
                cmpNext2(*node_to_process, AssignmentOperator)
            ) {
                AstPtr assignment_contents = next2(*node_to_process);
                do { /* Loop until we find a semicolon */
                    /* `copyAstPtr` doesn't include children and we need children of `[`, `(`, and `{` */
                    AstPtr assignment_contents_with_children = copyAstPtr(assignment_contents);
                    swapChildren(assignment_contents_with_children, assignment_contents);
                    addChild(processed_node, assignment_contents_with_children);
                    assignment_contents = assignment_contents->next;
                } while (
                    assignment_contents != NULL &&
                    assignment_contents->node_type != StatementEnd
                );
                addChild(processed_node, copyAstPtr(assignment_contents));
                processed_node->node_type = VariableAssignment;
                (*node_to_process) = assignment_contents;
            }
        }
    }

    if (node_type == Identifier) { /* IDENTIFIER */
        addChild(processed_node, copy_node);
        if (
            cmpNext1(*node_to_process, ParenExprBegin) /* IDENTIFIER ( */
        ) {
            addChild(processed_node, copyAstPtr(next1(*node_to_process)));
            processed_node->node_type = FunctionCall;
            (*node_to_process) = next1(*node_to_process);
        }

        else if ( /* IDENTIFIER = ... ; */
            cmpNext1(*node_to_process, AssignmentOperator) ||
            cmpNext1(*node_to_process, Operator)
        ) {
            AstPtr assignment_contents = next1(*node_to_process);
            do { /* Loop until we find a semicolon */
                /* `copyAstPtr` doesn't include children and we need children of `[`, `(`, and `{` */
                AstPtr assignment_contents_with_children = copyAstPtr(assignment_contents);
                swapChildren(assignment_contents_with_children, assignment_contents);
                addChild(processed_node, assignment_contents_with_children);
                assignment_contents = assignment_contents->next;
            } while (
                assignment_contents != NULL &&
                assignment_contents->node_type != StatementEnd
            );
            processed_node->node_type = Expression;
            (*node_to_process) = assignment_contents;
        }
    }

    /* Clean-up and return */
    if (processed_node->node_type != InvalidNode) return processed_node;
    delAstPtr(&processed_node);
    return copy_node;
}

static void buildSecondPassTree(AstPtr current_astp, AstPtr node_to_process) {
    AstPtr old_parent_astp = current_astp;

    /* Determine the type of node using its neighbors to really hone it in */
    // printf("Processing(%s)\n", node_to_process->tokens[0].text);
    AstPtr processed_node = processAstNode(&node_to_process);
    addChild(current_astp, processed_node);

    /* If I have any neighbors who weren't processed, process them here.
        `processAstNode` will SKIP neighbors who were used to hone in a previous node.
        Ex: `(` being used to prove `func` is a function header -> so we skip past `(` */
    AstPtr next = node_to_process->next;
    if (next) {
        // printf("NEXT(%s)...\n", next->tokens[0].text);
        buildSecondPassTree(current_astp, next);
    }
    current_astp = old_parent_astp;

    /* If I have any children, they haven't been processed yet, so process them here */
    current_astp = processed_node;
    AstPtr child = node_to_process->child;
    if (child) {
        // printf("CHILD(%s)...\n", child->tokens[0].text);
        buildSecondPassTree(current_astp, child);
    }
    current_astp = old_parent_astp;
}

AstPtr buildAstTree(const char* file_path, const FileLine file_as_lines[CASPIAN_MAX_LINES_IN_FILE], const uint num_file_lines) {
    
    /* Create a hierarchical tree for the nodes in the AST linked-list */
    AstPtr first_pass_tree = buildFirstPassTree(file_path, file_as_lines, num_file_lines);
    printf("\nFirstPass:\n");
    treeAstPtr(first_pass_tree, 0);
    printf("\n");

    // delAstPtr(&first_pass_tree);
    // return NULL;

    /* Setup for the 2nd hierarchical tree */
    generateMasterTokens(file_path, MASTER_TOKENS);
    AstPtr master_astp     = newAstPtr(MASTER_TOKENS, 1);
    master_astp->node_type = Namespace;
    gCurrentAstMaster      = master_astp;

    /* Copy the `first_pass_tree` but properly tag all nodes in the tree */
    AstPtr current_astp = master_astp;
    buildSecondPassTree(current_astp, first_pass_tree->child);
    printf("\nSecondPass:\n");
    treeAstPtr(master_astp, 0);
    printf("\n");

    /* Clean-up */
    delAstPtr(&first_pass_tree); /* No longer used */
    delAstPtr(&master_astp); // TODO: Remove, only for testing
    master_astp = NULL;

    return master_astp;
}