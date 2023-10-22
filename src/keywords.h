#ifndef CASPIAN_KEYWORDS_H
#define CASPIAN_KEYWORDS_H

#include <stdbool.h>
#include "lexer.h"

bool isKeyword(const Token* token);
bool isConditional(const Token* token);
bool isLoop(const Token* token);

#endif /* CASPIAN_KEYWORDS_H */