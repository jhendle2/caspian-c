#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "error.h"
#include "ast.h"
// #include "keywords.h"
// #include "parser.h"
#include "types.h"
#include "assembler.h"

ExitCode processFile(const char* file_path) {

    FileLine file_as_lines[CASPIAN_MAX_LINES_IN_FILE];
    uint num_file_lines = readFileAsLines(file_path, file_as_lines);

    TokenList file_as_tokens = buildTokenListFromLines(file_path, file_as_lines, num_file_lines);
    // printTokenList( file_as_tokens); printf("\n");

    AstPtr master_node = buildAstTree(file_as_tokens);
    printf("\n[Tree]\n");
    treeAstPtr(master_node, 0);

    char file_no_ext[CASPIAN_MAX_PATH_SZ-2];
    strncpy(file_no_ext, file_path, CASPIAN_MAX_PATH_SZ-2);
    file_no_ext[strlen(file_no_ext)-2] = 0;

    char asm_path[CASPIAN_MAX_PATH_SZ];
    snprintf(asm_path, CASPIAN_MAX_PATH_SZ, "%s.s", file_no_ext);
    gActiveAsm = fopen(asm_path, "w");

    assembleFromAST(gActiveAsm, master_node);
    if (gActiveAsm) fclose(gActiveAsm);

    delTokenList  (&file_as_tokens);
    delAstPtr     (&master_node);

    char cmd_assemble[256];
    char cmd_link[256];

    sprintf(cmd_assemble, "nasm -f elf64 -l %s.lst %s", file_no_ext, asm_path);
    sprintf(cmd_link    , "gcc -z noexecstack -m64 -no-pie -o %s %s.o", file_no_ext, file_no_ext); 

    int ret = system(cmd_assemble);
        ret = system(cmd_link);
    if (ret) printf("[ERRO] error while linking.\n");

    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
    if (argc<2) error(1, "No input file provided");
    const char* file_path = argv[1];

    ExitCode exit_code = processFile(file_path);
    return exit_code;
}