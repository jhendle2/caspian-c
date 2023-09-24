#ifndef CASPIAN_ERROR_H
#define CASPIAN_ERROR_H

#include <stdio.h>
#include <stdlib.h>

#include "color.h"
#include "lexer.h"

#define error(EXIT_CODE, ...)                         { printf( RED     "\n[ERRO] " RESET __VA_ARGS__);                                                              printf("  \n"); exit(EXIT_CODE); }
#define error_line(EXIT_CODE, FILELINE, ...)          { printf( RED     "\n[ERRO] " RESET __VA_ARGS__); printf("\n"); printFileLine(&FILELINE);                      printf("  \n"); exit(EXIT_CODE); }
#define error_token(EXIT_CODE, TOKEN, ...)            { printf( RED     "\n[ERRO] " RESET __VA_ARGS__); printf("\n"); printToken(&TOKEN)      ;                      printf("  \n"); exit(EXIT_CODE); }
#define error_token_2(EXIT_CODE, TOKEN1, TOKEN2, ...) { printf( RED     "\n[ERRO] " RESET __VA_ARGS__); printf("\n"); printToken(&TOKEN1)     ; printToken(&TOKEN2); printf("  \n"); exit(EXIT_CODE); }

#define warning(...)                                  { printf(BMAGENTA "\n[WARN] " RESET __VA_ARGS__);                                                              printf("\n\n");                  }
#define warning_line(FILELINE, ...)                   { printf(BMAGENTA "\n[WARN] " RESET __VA_ARGS__); printf("\n"); printFileLine(&FILELINE);                      printf("\n\n");                  }
#define warning_token(TOKEN, ...)                     { printf(BMAGENTA "\n[WARN] " RESET __VA_ARGS__); printf("\n"); printToken(&TOKEN)      ;                      printf("\n\n");                  }
#define warning_token_2(TOKEN1, TOKEN2, ...)          { printf(BMAGENTA "\n[WARN] " RESET __VA_ARGS__); printf("\n"); printToken(&TOKEN1)     ; printToken(&TOKEN2); printf("\n\n");                  }

typedef int ExitCode;

void safeExit();

#endif /* CASPIAN_ERROR_H */