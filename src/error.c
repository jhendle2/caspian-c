#include "error.h"

#include "parser.h"
// #include "ast.h"

void safeExit() {
    printf("[PREMATURE EXIT!]\n");

    /********************************************************************/
    extern SyntaxPtr gCurrentSyntaxMaster;
    delSyntaxPtr(&gCurrentSyntaxMaster);
    printf("Succesfully cleaned-up `gCurrentSyntaxMaster`\n");
    
    /********************************************************************/
}