#include <stdio.h>

#include "lexer.h"
#include "error.h"
#include "ast.h"
// #include "keywords.h"
// #include "parser.h"
#include "types.h"

ExitCode processFile(const char* file_path) {

    // TODO: Remove, only for testing tokenizer 
    FileLine file_as_lines[CASPIAN_MAX_FILELINE_SZ];
    uint num_file_lines = readFileAsLines(file_path, file_as_lines);

    // for (uint i = 0; i<num_file_lines; i++) {
    //     // printFileLine(file_as_lines+i);
    //     Token tokens[CASPIAN_MAX_TOKENS_IN_LINE];
    //     uint num_tokens = tokenizeLine(file_as_lines+i, tokens);
    //     if (num_tokens) {
    //         for (uint t = 0; t<num_tokens; t++) {
    //             printToken(tokens+t);
    //             // printf(" -- "); printFileLine(&(tokens[t].origin));
    //             // printf("\n");
    //         }
    //         printf("\n\n");
    //     }
    // }
    // return EXIT_SUCCESS;

    // AstPtr master_astp = buildAstTree(file_path);
    // masterTreeAstPtr(master_astp);
    // delAstPtr(&master_astp);

    // FileLine alias_line = newFileLine(0, "", "const int***");

    // Token key_type = newToken(11, &file_as_lines[0], "i32");
    // printf("key=%s\n", key_type.text);

    // Token alias_tokens[CASPIAN_MAX_TOKENS_IN_LINE];
    // uint alias_len = tokenizeLine(&alias_line, alias_tokens);
    // addTypeAlias(&key_type, alias_tokens, alias_len);

    // Token recall_tokens[CASPIAN_MAX_TOKENS_IN_TYPE_RECALL];
    // uint recall_len = 0;
    // TypeStatus recall_status = recallTypeAlias(&key_type, recall_tokens, &recall_len);

    // printf("recall="); printTokens(recall_tokens, recall_len);

    // FileLine line = newFileLine(0, "", "bob");
    // Token tokens[CASPIAN_MAX_TOKENS_IN_LINE];
    // tokenizeLine(&line, tokens);
    // printf("%s is type? %s\n", tokens[0].text, isType(&(tokens[0]))?"YES":"NO");

    // return EXIT_SUCCESS;

    // printf("\n---- ---- ---- ----\n");

    // SyntaxPtr master_sp = buildSyntaxTree(file_path, file_as_lines, num_file_lines);
    // treeSyntaxPtr(master_sp, 0);

    // printf("\n---- ---- ---- ----\n");

    // AstPtr master_astp = buildAstTree(file_path, master_sp);
    // treeAstPtr(master_astp, 0);

    // printf("\n---- ---- ---- ----\n");

    // delSyntaxPtr (&master_sp  );
    // delAstPtr    (&master_astp);

    // AstPtr master = buildAstTree(file_path, file_as_lines, num_file_lines);
    // delAstPtr(&master);

    TokenList file_as_tokens = buildTokenListFromLines(file_path, file_as_lines, num_file_lines);
    printTokenList( file_as_tokens); printf("\n");
    delTokenList  (&file_as_tokens);

    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
    if (argc<2) error(1, "No input file provided");
    const char* file_path = argv[1];

    ExitCode exit_code = processFile(file_path);
    return exit_code;
}