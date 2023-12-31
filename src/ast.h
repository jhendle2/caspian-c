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
    ParmsBegin,
    ParmDeclaration,
    ParmsEnd,
    ArgsBegin,
    ArgsEnd,

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
    SysCall,
    ReturnType,

    Operator,
    AssignmentOperator,
    
    VariableDeclaration,
    VariableDefinition,
    VariableAssignment,
    Expression,

    Conditional,
    Switch,
    Loop,
};

typedef struct token_list_s {
    Token token;
    struct token_list_s *prev, *next;
} *TokenList;

TokenList newTokenList     (const Token* token);
void      pushBackTokenList(TokenList head, TokenList item);
void      delTokenList     (TokenList* head);
void      printTokenList   (TokenList head);
TokenList pluckTokenList   (TokenList item);
TokenList popBackTokenList (TokenList head);
TokenList popFrontTokenList(TokenList head);
void      swapTokenList    (TokenList a, TokenList b);
TokenList moveNewTokenList (TokenList* original);

TokenList buildTokenListFromLines(const char* file_path, const FileLine file_as_lines[CASPIAN_MAX_LINES_IN_FILE], const uint num_file_lines);

/***************************************************************************************/

#define CASPIAN_MAX_AST_CHILDREN 128

typedef struct AstNode {
    enum AstNodeType node_type;
    uint             num_children;
    TokenList        tokens;
    struct AstNode  *parent;
    struct AstNode  *prev, *next;
    struct AstNode  *children;
} *AstPtr;

AstPtr newAstPtr     (const Token* token);
void   delAstPtr     (AstPtr* astp);
void   printAstPtr   (const AstPtr astp);
void   treeAstPtr    (const AstPtr astp, const uint level);
AstPtr getLastAstPtr (const AstPtr head);
void   appendAstPtr  (AstPtr head  , AstPtr next);
void   addChildAstPtr(AstPtr parent, AstPtr child);
AstPtr pluckAstPtr   (AstPtr astp);
bool   cmpAstPtr     (AstPtr astp, const char* token);

// /***************************************************************************************/

AstPtr buildAstTree(TokenList file_as_tokens);

#endif /* CASPIAN_AST_H */