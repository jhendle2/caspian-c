#include <stdio.h>

#include "error.h"
#include "ast.h"
#include "keywords.h"

ExitCode processFile(const char* file_path) {
    AstPtr master_astp = buildAstTree(file_path);
    // masterTreeAstPtr(master_astp);

    delAstPtr(&master_astp);

    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
    if (argc<2) error(1, "No input file provided");
    const char* file_path = argv[1];

    ExitCode exit_code = processFile(file_path);
    return exit_code;
}