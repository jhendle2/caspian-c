#include <stdio.h>

#include "color.h"
#include "lexer.h"
#include "error.h"
#include "parser.h"

ExitCode processFile(const char* file_path) {
    FileLine file_as_lines[CASPIAN_MAX_LINES_IN_FILE];
    uint num_file_lines = readFileAsLines(file_path, file_as_lines);

    SyntaxPtr master = buildSyntaxTree(file_path, file_as_lines, num_file_lines);
    masterTreeSyntaxPtr(master);

    delSyntaxPtr(&master);

    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
    if (argc<2) error(1, "No input file provided");
    const char* file_path = argv[1];

    ExitCode exit_code = processFile(file_path);
    return exit_code;
}