#ifndef CASPIAN_ERROR_H
#define CASPIAN_ERROR_H

#include <stdlib.h>

#include "color.h"
#include "lexer.h"

#define error(EXIT_CODE, ...)                { printf( RED     "[ERRO] " RESET __VA_ARGS__); printf("\n");                           exit(EXIT_CODE); }
#define error_line(EXIT_CODE, FILELINE, ...) { printf( RED     "[ERRO] " RESET __VA_ARGS__); printf("\n"); printFileLine(&FILELINE); exit(EXIT_CODE); }
#define error_token(EXIT_CODE, TOKEN, ...)   { printf( RED     "[ERRO] " RESET __VA_ARGS__); printf("\n"); printToken(&TOKEN)      ; exit(EXIT_CODE); }

#define warning(...)                         { printf(BMAGENTA "[WARN] " RESET __VA_ARGS__); printf("\n\n"); }
#define warning_line(FILELINE, ...)          { printf(BMAGENTA "[WARN] " RESET __VA_ARGS__); printf("\n"); printFileLine(&FILELINE); printf("\n"); }
#define warning_token(TOKEN, ...)            { printf(BMAGENTA "[WARN] " RESET __VA_ARGS__); printf("\n"); printToken(&TOKEN)      ; printf("\n"); }

typedef int ExitCode;

#endif /* CASPIAN_ERROR_H */