#ifndef CASPIAN_AST_H
#define CASPIAN_AST_H

#include "lexer.h"

#define CASPIAN_MAX_AST_CHILDREN 128

enum AstNodeType {
    InvalidNode,

    Namespace,
    TypeReference,
    Identifier,

    StatementEnd,
    ParenExprBegin,
    ListContinue,
    ParenExprEnd,
    BlockBegin,
    BlockEnd,
    IndexBegin,
    IndexEnd,

    FunctionModifier, /* static, inline */
    TypeModifier,     /* const */

    StructDecl,
    UnionDecl,
    EnumDecl,
    AliasDecl,

    IntegerConst,
    FloatConst,
    CharacterConst,
    StringConst,

    ReturnStatement,

    FunctionDeclaration,
    Function,
    FunctionCall,

    Operator,
    AssignmentOperator,
    
    VariableDeclaration,
    VariableAssignment,
    Expression,
};

typedef struct token_list_s {
    Token token;
    struct token_list_s *prev, *next;
} *TokenList;

TokenList newTokenList   (const Token* token);
void    pushBackTokenList(TokenList head, TokenList item);
void    delTokenList     (TokenList* head);
void    printTokenList   (TokenList head);

TokenList buildTokenListFromLines(const char* file_path, const FileLine file_as_lines[CASPIAN_MAX_LINES_IN_FILE], const uint num_file_lines);

/***************************************************************************************/

// typedef struct AstNode {
//     AstNodeType     node_type;
//     uint            num_tokens;
//     Token           tokens[CASPIAN_MAX_TOKENS_IN_LINE];
//     struct AstNode *parent, *child;
// } *AstPtr;

// AstPtr newAstPtr  (const Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], const uint num_tokens);
// void   delAstPtr  (AstPtr* astp);
// void   printAstPtr(const AstPtr astp);
// void   treeAstPtr (const AstPtr astp, const uint level);

// /***************************************************************************************/

// AstPtr buildAstTree(const char* file_path, const FileLine file_as_lines[CASPIAN_MAX_LINES_IN_FILE], const uint num_file_lines);

#endif /* CASPIAN_AST_H */