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

typedef struct AstNode {
    AstNodeType     node_type;
    uint            num_tokens;
    Token           tokens[CASPIAN_MAX_TOKENS_IN_LINE];
    struct AstNode *parent, *child, *prev, *next;
} *AstPtr;

AstPtr newAstPtr  (const Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], const uint num_tokens);
void   delAstPtr  (AstPtr* astp);
void   printAstPtr(const AstPtr astp);
// void   dumpAstPtr(const AstPtr astp);
void   appendAstPtr(AstPtr astp, AstPtr next);
void   treeAstPtr (const AstPtr astp, const uint level);

/***************************************************************************************/

AstPtr buildAstTree(const char* file_path, const FileLine file_as_lines[CASPIAN_MAX_LINES_IN_FILE], const uint num_file_lines);

#endif /* CASPIAN_AST_H */