#include <stdio.h>

#include "lexer.h"
#include "error.h"
// #include "ast.h"
#include "keywords.h"
#include "parser2.h"

ExitCode processFile(const char* file_path) {

    // TODO: Remove, only for testing tokenizer 
    FileLine file_as_lines[CASPIAN_MAX_FILELINE_SZ];
    uint num_file_lines = readFileAsLines(file_path, file_as_lines);

    // for (uint i = 0; i<num_file_lines; i++) {
    //     printFileLine(file_as_lines+i);
    //     Token tokens[CASPIAN_MAX_TOKENS_IN_LINE];
    //     uint num_tokens = tokenizeLine(file_as_lines+i, tokens);
    //     for (uint t = 0; t<num_tokens; t++) {
    //         printToken(tokens+t);
    //         printf(" -- "); printFileLine(tokens[t].origin);
    //         printf("\n");
    //     }
    //     printf("\n\n");
    // }

    // AstPtr master_astp = buildAstTree(file_path);
    // masterTreeAstPtr(master_astp);

    // delAstPtr(&master_astp);

    SyntaxPtr master = buildSyntaxTree(file_path, file_as_lines, num_file_lines);
    treeSyntaxPtr(master, 0);
    delSyntaxPtr (&master);

    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
    if (argc<2) error(1, "No input file provided");
    const char* file_path = argv[1];

    ExitCode exit_code = processFile(file_path);
    return exit_code;
}