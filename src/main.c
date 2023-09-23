#include <stdio.h>

#include "color.h"
#include "lexer.h"
#include "error.h"

int main(int argc, char** argv) {
    if (argc<2) error(1, "No input file provided");
    const char* file_path = argv[1];

    FileLine file_as_lines[CASPIAN_MAX_LINES_IN_FILE];
    uint num_file_lines = readFileAsLines(file_path, file_as_lines);

    for (uint i = 0; i<num_file_lines; i++) {
        printFileLine(file_as_lines+i);
        Token tokens[CASPIAN_MAX_TOKENS_IN_LINE];
        uint num_tokens = tokenizeLine(file_as_lines+i, tokens);
        for (uint j = 0; j<num_tokens; j++)
            printToken(tokens+j);
        printf("\n");
    }

    return 0;
}