#include "sys_call.h"

/* Grabbed from here: https://filippo.io/linux-syscall-table/ */

SysCallId getSysCallId(const Token* token) {
    if (cmpToken(token, "write")) return 1;
    // if (cmpToken(token, "printf")) return 1;
    return NOT_A_SYS_CALL;
}