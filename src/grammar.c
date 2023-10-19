#include "grammar.h"

#include <stdio.h>

#include "utils.h"
#include "types.h"
#include "error.h"

#define next1(ASTP) (      ASTP->next  ?       ASTP->next  : NULL)
#define next2(ASTP) (next1(ASTP->next) ? next1(ASTP->next) : NULL)
#define next3(ASTP) (next2(ASTP->next) ? next2(ASTP->next) : NULL)
#define next4(ASTP) (next3(ASTP->next) ? next3(ASTP->next) : NULL)
#define next5(ASTP) (next4(ASTP->next) ? next4(ASTP->next) : NULL)

static bool isPossibleIdentifier(const Token* token) {
    return !(
        isType(token) ||
        isOperatorDelimiter(token) ||
        isDec(token->text[0])
    );
}

static bool isTypeOrDescriptor(const Token* token) {
    if (cmpToken(token, "*"      )) return true;
    if (cmpToken(token, "static" )) return true;
    if (cmpToken(token, "const"  )) return true;
    if (cmpToken(token, "inline" )) return true;
    return isType(token);
}

static bool typeConcatenator(AstPtr* type_builder) {
    bool is_concatenated_type = false;
    AstPtr possible_descriptor = (*type_builder)->next;
    while (possible_descriptor && isTypeOrDescriptor(&(possible_descriptor->tokens->token))) {
        is_concatenated_type = true;
        AstPtr next = possible_descriptor->next;

        TokenList descriptor_token = moveNewTokenList(&(possible_descriptor->tokens));
        pushBackTokenList( (*type_builder)->tokens, descriptor_token );

                   possible_descriptor = pluckAstPtr(possible_descriptor);
        delAstPtr(&possible_descriptor);

        possible_descriptor = next;
    } return is_concatenated_type;
}

#include <assert.h>
void discernNodeType(AstPtr* head_ptr) {
    AstPtr head = *head_ptr;
    const Token first = head->tokens->token;

    if (isInteger  (&first))         { head->node_type = IntegerConst  ; return; }
    if (isFloat    (&first))         { head->node_type = FloatConst    ; return; }
    if (isString   (&first))         { head->node_type = StringConst   ; return; }
    if (isCharacter(&first))         { head->node_type = CharacterConst; return; }

    if (head->node_type==InvalidNode && isOperatorDelimiter(&first)) { head->node_type = Operator; return; }

    const bool is_concatenated_type = typeConcatenator(head_ptr);
    if (is_concatenated_type || isType(&first)) {
        AstPtr identifier  = next1(head);

        /* Possible function header */
        AstPtr parms_begin = next2(head);
        AstPtr block_begin = next3(head);
        if (
            isPossibleIdentifier(&(identifier ->tokens->token)     ) &&
                        cmpToken(&(parms_begin->tokens->token), "(") &&
                        cmpToken(&(block_begin->tokens->token), "{")
        ) {
            /* Function */
            AstPtr function = head;
            function->node_type = FunctionDeclaration;

            /* Append name */
            TokenList function_name = moveNewTokenList(&(identifier->tokens));
            pushBackTokenList(function->tokens, function_name);
                       identifier = pluckAstPtr(identifier);
            delAstPtr(&identifier);

            /* Parms */
            AstPtr parms_end = getLastAstPtr(parms_begin->children);
            // if (cmpAstPtr(parms_end, ")")==false)
            //     error_token(1, parms_begin->tokens->token, "Parameter list missing its closing parenthesis");
            parms_begin = pluckAstPtr(parms_begin);
            parms_begin->node_type = ParmsBegin;

            AstPtr parms = parms_begin->children;
            // AstPtr current_parm = NULL;
            // TokenList parm_tokens = NULL;
            while (parms) {
                if (cmpAstPtr(parms, ")")) break;
                printf("parm: %s\n", parms->tokens->token.text);

                AstPtr next = parms->next;
                
                if (cmpAstPtr(parms, ",")) {
                    // FIXME: ???
                }

                printf("discerning [%s]\n", parms->tokens->token.text);
                discernNodeType(&parms);
                parms = next;
            }
            // if (current_parm) addChildAstPtr(parms_begin, current_parm);

            parms_end->node_type = ParmsEnd  ;
            addChildAstPtr(function, parms_begin);

            /* Block */
            AstPtr block_end = getLastAstPtr(block_begin->children);
            // if (cmpAstPtr(block_end, "}")==false)
            //     error_token(1, block_begin->tokens->token, "Function block missing its closing brace");
                   block_begin = pluckAstPtr  (block_begin);
            block_begin->node_type = BlockBegin;
            block_end  ->node_type = BlockEnd  ;
            addChildAstPtr(function, block_begin);

            return;
        }

        /* Possible variable declaration */
        AstPtr semicolon = next2(head);
        if (
            isPossibleIdentifier(&(identifier ->tokens->token)     ) &&
                (
                    cmpToken(&(semicolon->tokens->token), ";") ||
                    cmpToken(&(semicolon->tokens->token), ",") ||
                    semicolon->node_type==ParmsEnd
                )
        ) {
            /* Varaible-Declaration */
            AstPtr variable = head;
            variable->node_type = VariableDeclaration;
            if (semicolon->node_type==ParmsEnd) variable->node_type = ParmDeclaration;


            /* Append name */
            TokenList variable_name = moveNewTokenList(&(identifier->tokens));
            pushBackTokenList(variable->tokens, variable_name);
                       identifier = pluckAstPtr(identifier);
            delAstPtr(&identifier);

                       semicolon = pluckAstPtr(semicolon);
            delAstPtr(&semicolon);

            return;
        }

        /* Possible variable definition */
        AstPtr equals = next2(head);
        if (
            isPossibleIdentifier(&(identifier ->tokens->token)) &&
                cmpToken(&(equals->tokens->token), "=")
        ) {
            /* Varaible-Declaration */
            AstPtr variable = head;
            variable->node_type = VariableDefinition;

            /* Append name */
            TokenList variable_name = moveNewTokenList(&(identifier->tokens));
            pushBackTokenList(variable->tokens, variable_name);
                       identifier = pluckAstPtr(identifier);
            delAstPtr(&identifier);

            /* And copy the expression in as my children */
            AstPtr until_semicolon = equals;
            while (until_semicolon) {
                if (cmpToken(&(until_semicolon->tokens->token), ";")) break;
                AstPtr expressand = until_semicolon;
                until_semicolon   = until_semicolon->next;
                       expressand = pluckAstPtr(expressand);
                discernNodeType(&expressand);
                addChildAstPtr(variable, expressand);
            }
                       until_semicolon = pluckAstPtr(until_semicolon);
            delAstPtr(&until_semicolon);

            return;
        }
    }

    AstPtr return_kw = head;
    if (cmpToken(&first, "return")) {
        return_kw->node_type = ReturnStatement;

        AstPtr until_semicolon = next1(return_kw);
        while (until_semicolon) {
            if (cmpToken(&(until_semicolon->tokens->token), ";")) break;
            AstPtr expressand = until_semicolon;
            until_semicolon   = until_semicolon->next;
                    expressand = pluckAstPtr(expressand);
            discernNodeType(&expressand);
            addChildAstPtr(return_kw, expressand);
        }
                    until_semicolon = pluckAstPtr(until_semicolon);
        delAstPtr(&until_semicolon);
        return;
    }

    AstPtr function_call = head;
    AstPtr args_begin    = next1(function_call);
    if (
        isPossibleIdentifier(&(function_call ->tokens->token)) &&
            cmpAstPtr(args_begin, "(")
    ) {
        function_call->node_type = FunctionCall;
        args_begin->node_type = ArgsBegin;
        AstPtr args_end = getLastAstPtr(args_begin->children);
        // if (cmpAstPtr(args_end, ")")==false)
        //         error_token(1, args_begin->tokens->token, "Arguments list missing its closing parenthesis");
        args_end->node_type = ArgsEnd;
        return;
    }


    AstPtr possible_variable = head;
    if (isPossibleIdentifier(&(function_call ->tokens->token))) {
        possible_variable->node_type = Identifier;
        return;
    }

    if (cmpToken(&first, "{")) {}
}