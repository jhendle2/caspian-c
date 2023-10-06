#include "error.h"

// #include "parser.h"
#include "ast.h"

void safeExit(const ExitCode exit_code) {
    printf("[[[PREMATURE EXIT!]]]\n");

    /********************************************************************/
    extern TokenList
            gTokenListMaster    ,
            gTokenListStream    ,
            gTokenListScopeStack,
            gTokenListLastPopped;
        
    delTokenList(&gTokenListMaster);
    delTokenList(&gTokenListStream);
    delTokenList(&gTokenListScopeStack);
    delTokenList(&gTokenListLastPopped);
    printf("Succesfully cleaned-up all dangling token-lists\n");


    /********************************************************************/
    // extern SyntaxPtr gCurrentSyntaxMaster;
    // extern uint gTotalSyntaxNodes, gTotalSyntaxFrees;

    // if (gTotalSyntaxNodes > 0) {
    //     delSyntaxPtr(&gCurrentSyntaxMaster);
    //     printf("Succesfully cleaned-up [%u/%u] in `gCurrentSyntaxMaster`\n", gTotalSyntaxFrees, gTotalSyntaxNodes);
    // } else {
    //     printf("No clean-up necessary for parser\n");
    // }
    
    /********************************************************************/
    // extern AstPtr gCurrentAstMaster;
    // extern uint gTotalAstNodes, gTotalAstFrees;
    // delAstPtr(&gCurrentAstMaster);
    // printf("Succesfully cleaned-up [%u/%u] in `gCurrentAstMaster`\n", gTotalAstFrees, gTotalAstNodes);

    /********************************************************************/
    
    exit(exit_code);
}