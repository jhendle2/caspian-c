// #ifndef CASPIAN_AST_H
// #define CASPIAN_AST_H

// #include "lexer.h"
// #include "parser.h"

// #define CASPIAN_MAX_AST_CHILDREN 512

// enum AstOp {
//     AST_INVALID,
//     AST_MASTER,
//     AST_NAMESPACE,

//     AST_VAR_INIT,

//     AST_FOR_LOOP,
//     AST_WHILE_LOOP,

//     AST_CONTINUE,
//     AST_BREAK,
//     AST_GOTO,

//     AST_IF,
//     AST_ELSE,

//     AST_STRUCT,
//     AST_ENUM,
//     AST_UNION,
    
//     AST_TYPE_ALIAS,

//     AST_FUNCTION_HEADER,
//     AST_FUNCTION_CALL,
//     AST_RETURN,

//     AST_INTEGER_CONSTANT,
//     AST_STRING_CONSTANT,

//     AST_CAST,
// NUM_AST_OPS
// };

// typedef struct ast_node_s {
//     AstOp op;

//     bool has_identifier;
//     Token identifier;

//     uint num_tokens, num_children;
//     Token tokens[CASPIAN_MAX_TOKENS_IN_LINE];

//     struct ast_node_s* parent;
//     struct ast_node_s* children[CASPIAN_MAX_AST_CHILDREN];
// } *AstPtr;

// AstPtr newAstPtr  (const enum AstOp op, const Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], const uint num_tokens);
// void   delAstPtr  (AstPtr* astp);
// void   printAstPtr(const AstPtr astp);
// void   treeAstPtr (const AstPtr astp, const uint level);
// void   assignIdentifier(AstPtr astp, const Token identifier);
// #define masterTreeAstPtr(MASTER) treeAstPtr(MASTER, 0)

// AstPtr buildAstTree(const char* file_path);

// #endif /* CASPIAN_PARSER_H */