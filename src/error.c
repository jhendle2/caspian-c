#include "error.h"

// #include "parser.h"
#include "ast.h"

void safeExit(const ExitCode exit_code) {
    printf("[[[PREMATURE EXIT!]]]\n");

    /********************************************************************/
    extern TokenList
            gTokenListMaster    ,
            gTokenListStream    ,
            gTokenListScopeStack;
            // gTokenListLastPopped;
        
    delTokenList(&gTokenListMaster);
    delTokenList(&gTokenListStream);
    delTokenList(&gTokenListScopeStack);
    // delTokenList(&gTokenListLastPopped); /* Not needed for some reason? */
    printf("Succesfully cleaned-up all dangling token-lists\n");


    /********************************************************************/
    extern AstPtr gAstMaster;
    delAstPtr(&gAstMaster);
    printf("Successfully cleaned-up all dangling AST-nodes\n");

    /********************************************************************/
    extern FILE* gActiveAsm;
    if (gActiveAsm) fclose(gActiveAsm);
    printf("Successfully cleaned-up the active assembly outfile\n");

    /********************************************************************/
    exit(exit_code);
}