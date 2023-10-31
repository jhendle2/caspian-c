#include "assembler.h"

#include <string.h>
#include <stdlib.h>

#include "sys_call.h"
#include "error.h"
#include "constants.h"

FILE* gActiveAsm;

static bool isImmediate(const enum AstNodeType node_type) {
    return (
        node_type == IntegerConst   ||
        node_type == CharacterConst
    );
}

static uint evaluateImmediate(const AstPtr immediate) {
    const char* immed_str = immediate->tokens->token.text;
    if (immediate->node_type == IntegerConst) return atoi(immed_str);
    else return (uint)(immed_str[1]);
}

typedef struct {
    #define CASPIAN_MAX_ASM_WRITE_BLOCK_SZ 512
    char header[CASPIAN_MAX_ASM_WRITE_BLOCK_SZ];
    char middle[CASPIAN_MAX_ASM_WRITE_BLOCK_SZ];
    char footer[CASPIAN_MAX_ASM_WRITE_BLOCK_SZ];
} AsmWriteBlock;

static AsmWriteBlock newAsmWriteBlock() {
    AsmWriteBlock asm_block = {"", "", ""};
    return asm_block;
}

AsmWriteBlock gSegmentData;
AsmWriteBlock gSegmentRoData;
// AsmWriteBlock gSegmentBSS;

static AsmConstant addToSegment(AsmWriteBlock* segment, const AstPtr constant) {
    const enum AstNodeType node_type = constant->node_type;
    const Token token = constant->tokens->token;
    printf("Identifying Constant(%s)\n", token.text);

    AsmConstant prev_asm_const;
    if (getAsmConstant(&token, &prev_asm_const) == true) return prev_asm_const;

    AsmConstant new_asm_const = {
        .id=0,
        .token=repairAsmStringConst(&token),
        .type=ACT_BYTE,
        .data_identifier=""
    };

    AsmWriteBlock constant_segment = newAsmWriteBlock();
    if (node_type == StringConst) {
        printf("String Constant(%s)\n", token.text);
        static uint constant_id = 0;
        new_asm_const.id = constant_id++;
        new_asm_const.type = ACT_DWORD;
        sprintf(new_asm_const.data_identifier, "const%u", new_asm_const.id);
        sprintf(constant_segment.middle, "\t%s: db %s\n\t%s_len: equ $ - %s\n",
                new_asm_const.data_identifier,
                new_asm_const.token.text, new_asm_const.data_identifier, new_asm_const.data_identifier);
        strcat(segment->middle, constant_segment.middle);
        printf("Constant-ID=%u\n", constant_id);
        // return constant_id;
    }
    else strncpy(new_asm_const.data_identifier, token.text, 32);

    addAsmConstant(&new_asm_const);
    return new_asm_const; /* Integer constants we don't care for their ID */
}

static void assembleExpression(AsmWriteBlock* asm_block, const AstPtr expression) {
    printf("Assembling Expression: "); printTokenList(expression->tokens); printf("\n");

    if (expression->node_type == StringConst) {
        addToSegment(&gSegmentData, expression);
    }
}

static void assembleFunction(AsmWriteBlock* asm_block, const AstPtr fn) {
    AsmWriteBlock function_block = newAsmWriteBlock();
    printf("Assembling Function(%s)\n", fn->tokens->next->token.text);

    const char* fn_id = fn->tokens->next->token.text;
    sprintf(function_block.header, "%s:\n", fn_id);
    strcat(function_block.header, "\tpush rbp\t; Store old stack pointer\n");
    strcat(function_block.header, "\tmov  rbp, rsp\t; Load stack pointer from function call\n");

    strcat(function_block.footer, "\tpop  rbp\t; Restore stack pointer\n");
    strcat(function_block.footer, "\tret\t\t\t; Exit function\n"); // Could use `leave` as well

    strcat(asm_block->header, function_block.header);
    // strcat(asm_block->middle, function_block.middle);
    strcat(asm_block->footer, function_block.footer);
}

static void assembleReturn(AsmWriteBlock* asm_block, const AstPtr ret) {
    AsmWriteBlock return_block = newAsmWriteBlock();
    printf("Assembling Return: "); printTokenList(ret->tokens); printf("\n");

    AstPtr expression = ret->children;
    assembleExpression(&return_block, expression);

    if (isImmediate(expression->node_type)) {
        AstPtr immediate = expression;
        const uint immediate_value = evaluateImmediate(immediate);
        sprintf(return_block.middle,  "\tmov  eax, %d\t; Constant load (%s)\n", immediate_value, immediate->tokens->token.text);
        strcat(asm_block->middle, return_block.middle);
    }
    else {
        sprintf(return_block.middle, "\tmov  eax, 0\n"); // TODO: Should load expression register from above
        strcat(asm_block->middle, return_block.middle);
    }
}

enum gAsmRegister {
    REG_RAX=0,
    REG_RCX,
    REG_RDX,
    REG_RBX,
    REG_RSP,
    REG_RBP,
    REG_RSI,
    REG_RDI,
    REG_R08,
    REG_R09,
    REG_R10,
    REG_R11,
    REG_R12,
    REG_R13,
    REG_R14,
    REG_R15,
NUM_ASM_REGISTERS
};
enum gAsmArgRegister {
    ARG_RAX=0,
    ARG_RDI,
    ARG_RSI,
    ARG_RDX,
    ARG_RCX,
    ARG_R08,
    ARG_R09,
    ARG_R10,
    ARG_R11,
    ARG_R12,
    ARG_R13,
    ARG_R14,
    ARG_R15,
NUM_ASM_ARG_REGISTERS
};
typedef char RegisterConst[5];
static const RegisterConst gAsmRegisters[16][5] = {
    /* x86 */
    {"rax", "eax" , "ax"  , "ah", "al"  },
    {"rcx", "ecx" , "cx"  , "ch", "cl"  },
    {"rdx", "edx" , "dx"  , "dh", "dl"  },
    {"rbx", "ebx" , "bx"  , "bh", "bl"  },
    {"rsp", "esp" , "sp"  , "??", "spl" },
    {"rbp", "ebp" , "bp"  , "??", "bpl" },
    {"rsi", "esi" , "si"  , "??", "sil" },
    {"rdi", "edi" , "di"  , "??", "dil" },

    /* x86-64 */
    {"r8" , "r8d" , "r8w" , "??", "r8b" },
    {"r9" , "r9d" , "r9w" , "??", "r9b" },
    {"r10", "r10d", "r10w", "??", "r10b"},
    {"r11", "r11d", "r11w", "??", "r11b"},
    {"r12", "r12d", "r12w", "??", "r12b"},
    {"r13", "r13d", "r13w", "??", "r13b"},
    {"r14", "r14d", "r14w", "??", "r14b"},
    {"r15", "r15d", "r15w", "??", "r15b"}
};
static const RegisterConst gArgRegisters[13][5] = {
    {"rax", "eax" , "ax"  , "ah", "al"  }, /* For syscalls only */
    {"rdi", "edi" , "di"  , "??", "dil" },
    {"rsi", "esi" , "si"  , "??", "sil" },
    {"rdx", "edx" , "dx"  , "dh", "dl"  },
    {"rcx", "ecx" , "cx"  , "ch", "cl"  },
    {"r8" , "r8d" , "r8w" , "??", "r8b" },
    {"r9" , "r9d" , "r9w" , "??", "r9b" },
    {"r10", "r10d", "r10w", "??", "r10b"},
    {"r11", "r11d", "r11w", "??", "r11b"},
    {"r12", "r12d", "r12w", "??", "r12b"},
    {"r13", "r13d", "r13w", "??", "r13b"},
    {"r14", "r14d", "r14w", "??", "r14b"},
    {"r15", "r15d", "r15w", "??", "r15b"}
};

// static void assembleSysCall(AsmWriteBlock* asm_block, const AstPtr sys_call, const SysCallId sys_call_id, const Token* fn_name) {
//     // AsmWriteBlock sys_call_block = newAsmWriteBlock();
//     // sprintf(sys_call_block.header, "\tmov  rax, %u\t; Sys call to (%s)\n", sys_call_id, fn_name->text);
//     // strcat(asm_block->header, sys_call_block.header);

//     // // if 
//     //         // sprintf(sys_call_block.middle, "\tmov  rdi, 1\t; Sys call to stdin\n");

//     // //         break;

//     // //     default:
//     // //         break;
//     // // }
//     // /* Load parms */
//     // strcat(asm_block->middle, sys_call_block.middle);
//     // // if (fn_call->next) assembleExpression(asm_block, fn_call->next);

//     // sprintf(sys_call_block.middle, "\tsyscall\n");
//     // strcat(asm_block->middle, sys_call_block.middle);
// }

static void assembleFunctionCall(AsmWriteBlock* asm_block, const AstPtr fn_call) {
    AsmWriteBlock function_call_block = newAsmWriteBlock();
    printf("Function Call(%s)\n", fn_call->tokens->token.text);

    // const Token fn_name = ;
    char fn_name[32];
    strncpy(fn_name, fn_call->tokens->token.text, 32);
    
    const bool is_sys_call = (fn_call->node_type == SysCall);

    AstPtr arg = fn_call->next->children;
    if (arg == NULL)
        error_token(1, fn_call->tokens->token, "This should never happen. Investigate -Jonah");

    if (cmpToken(&(arg->tokens->token), ")") == false) { /* We have more than zero arguments */
        uint arg_index = (is_sys_call ? ARG_RAX : ARG_RDI);
        AsmWriteBlock arg_block = newAsmWriteBlock();
        while (arg->next) {
            if (cmpToken(&(arg->tokens->token), ",")) arg_index++;
            else {
                char arg_value[32];
                AsmConstant asm_const = addToSegment(&gSegmentData, arg);
                // if (arg->node_type == StringConst) {

                // }
                strncpy(arg_value, asm_const.token.text, 32);
                printf("[next arg %u `%s`]\n", arg_index, arg_value);
                
                sprintf(
                    arg_block.middle, "\tmov  %s, %s\t; arg[%u]: %s\n",
                    gArgRegisters[arg_index][0], asm_const.data_identifier,
                    arg_index, arg_value
                );
                strcat(function_call_block.middle, arg_block.middle);
                
                if (arg->node_type == StringConst) {
                    arg_index++;
                    sprintf(
                        arg_block.middle, "\tmov  %s, %s_len\t; arg[%u_len]: %s size\n",
                        gArgRegisters[arg_index][0], asm_const.data_identifier,
                        arg_index, arg_value
                    );
                    strcat(function_call_block.middle, arg_block.middle);
                }
            }
            arg=arg->next;
        }
    }

    if (is_sys_call)
        sprintf(function_call_block.footer, "\tsyscall\n");
    else
        sprintf(function_call_block.footer, "\tcall %s\t; call function `%s`\n", fn_name, fn_name);

    strcat(asm_block->header, function_call_block.header);
    strcat(asm_block->middle, function_call_block.middle);
    strcat(asm_block->footer, function_call_block.footer);
}

static void assembleFromNode(AsmWriteBlock* asm_block, const AstPtr current) {
    printf("\n--------\nAssembling: "); printTokenList(current->tokens); printf("\n");
    switch (current->node_type) {
        case FunctionDeclaration:
            assembleFunction(asm_block, current);
            break;

        case ReturnStatement:
            assembleReturn(asm_block, current);
            break;

        case FunctionCall:
        case SysCall:
            assembleFunctionCall(asm_block, current);
            break;

        // case StringConst:
        //     addToSegment(&gSegmentData, current);
        //     break;

        default: break;
    }
}

static void assembleFromASTHelper(FILE* asm_out, const AstPtr current) {
    AsmWriteBlock asm_block = newAsmWriteBlock();
    assembleFromNode(&asm_block, current);
    
    printf("HEAD:\n%s", asm_block.header);
    fprintf(asm_out, "%s", asm_block.header);

    AstPtr child = current->children;
    if (child) assembleFromASTHelper(asm_out, child);
    
    printf("MIDDLE:\n%s", asm_block.middle);
    fprintf(asm_out, "%s", asm_block.middle);

    printf("FOOTER:\n%s", asm_block.footer);
    fprintf(asm_out, "%s", asm_block.footer);
    printf("\n");

    AstPtr next = current->next;
    if (next) assembleFromASTHelper(asm_out, next);
}

void assembleFromAST(FILE* asm_out, const AstPtr master) {

    fprintf(asm_out, "global main\n");
    fprintf(asm_out, "\n");
    fprintf(asm_out, "section .text\n");
    fprintf(asm_out, "\n");

    assembleFromASTHelper(asm_out, master);

    fprintf(asm_out, "\n");
    fprintf(asm_out, "section .rodata\n");
    fprintf(asm_out, "%s\n", gSegmentRoData.middle);
    fprintf(asm_out, "section .data\n");
    fprintf(asm_out, "%s\n", gSegmentData.middle);
    fprintf(asm_out, "\n");
}