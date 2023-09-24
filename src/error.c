#include "error.h"

#include "parser.h"
#include "ast.h"

void safeExit(const ExitCode exit_code) {
    printf("[PREMATURE EXIT!]\n");

    /********************************************************************/
    extern SyntaxPtr gCurrentSyntaxMaster;
    delSyntaxPtr(&gCurrentSyntaxMaster);
    printf("Succesfully cleaned-up `gCurrentSyntaxMaster`\n");
    
    /********************************************************************/
    extern AstPtr gCurrentAstMaster;
    delAstPtr(&gCurrentAstMaster);
    printf("Succesfully cleaned-up `gCurrentAstMaster`\n");

    /********************************************************************/
    
    exit(exit_code);
}