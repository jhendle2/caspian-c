#ifndef CASPIAN_SYS_CALL_H
#define CASPIAN_SYS_CALL_H

#include <stdbool.h>

#include "lexer.h"

typedef unsigned int SysCallId;
#define NOT_A_SYS_CALL -1
SysCallId getSysCallId(const Token* token);

#define SysCallWrite 1
#define SysCallPrint 1

#endif /* CASPIAN_SYS_CALL_H */