#include "lexer.h"

#include <stdio.h>
#include <string.h>

#include "error.h"
#include "color.h"
#include "utils.h"

/****************************************************************************************************/
FileLine newFileLine(const uint line_number, const char* origin, const char* text) {
    FileLine fl = { .line_number = line_number };
    strncpy(fl.origin, origin, CASPIAN_MAX_PATH_SZ    );
    strncpy(fl.text  , text  , CASPIAN_MAX_FILELINE_SZ);
    return fl;
}

void printFileLine(const FileLine* fl) {
    printf(BGREEN "%s:%u: " RESET "%s\n", fl->origin, fl->line_number, fl->text);
}

uint readFileAsLines(const char* file_path, FileLine file_as_lines[CASPIAN_MAX_LINES_IN_FILE]) {
    uint num_lines_in_file = 0;

    FILE* fp = fopen(file_path, "r");
    if (fp == NULL) {
        error(EXIT_FAILURE, "No such file `%s`", file_path);
    }

    uint line_number = 0;
    char buf[CASPIAN_MAX_FILELINE_SZ];
    while ( fgets(buf, CASPIAN_MAX_FILELINE_SZ, fp) ) {
        line_number++;                 /* Every line number is tracked, even blank ones */

        char* stripped = buf;
        // char* stripped = lstrip(buf);  /* Drop the leading spaces so we can print properly later */
        replace(stripped, '\n', '\0'); /* Drop the newline at the end of the fileline for printing */
        if (empty(stripped)) continue; /* Ignore blank lines */

        file_as_lines[num_lines_in_file++] = newFileLine(line_number, file_path, stripped);
    }

    fclose(fp);
    return num_lines_in_file;
}

/****************************************************************************************************/
Token newToken(const uint offset, const FileLine* origin, const char* text) {
    Token token = { .offset=offset, .origin=origin };
    strncpy(token.text, text, CASPIAN_MAX_TOKEN_SZ);
    return token;
}

#define CASPIAN_MAX_TOKEN_HEADER_SZ (CASPIAN_MAX_PATH_SZ + 1 + CASPIAN_MAX_LINES_IN_FILE + 1 + CASPIAN_MAX_FILELINE_SZ + 2) /* Exact max we need */ 
void printToken(const Token* token) {
    const FileLine* fl = token->origin;
    char temp[CASPIAN_MAX_TOKEN_HEADER_SZ];
        snprintf(temp, CASPIAN_MAX_TOKEN_HEADER_SZ, "%s:%u:%u: ", fl->origin, fl->line_number, token->offset);
    const uint header_len = strlen(temp), token_len = strlen(token->text);
        printf(BGREEN "%s" RESET "%s\n", temp, fl->text);
        printf(BGREEN "%*c", header_len + token->offset - 1, ' '); /* Minus 1 because offset is counted started at 1, not 0 */
    for (uint i = 0; i<token_len; i++)
        printf("^");
    printf(RESET "\n");
}

static bool isValidEscape(const char c) {
    switch (c) {
        case 'n' : case 'r' : case 't' : 
        case 'f' : case 'b' : case 'v' : 
        case '\\': case '\'': case '\"': 
            return true;
        default: return false;
    }
}

static bool isDelimiter(const char c) {
    switch (c) {
        case '(': case ')': case '[': case ']':
        case '{': case '}': case '<': case '>':
        case '!': case '%': case '^': case '&':
        case '*': case '~': case ':': case ';':
        case ',': case '.': case '?': case '|':
        case '+': case '-': case '/': case '=':
            return true;
        default: return false;
    }
}

static bool isOperator(const char c, const char d) {
    return (
        (c=='=' && d=='=') ||
        (c=='<' && d=='=') ||
        (c=='>' && d=='=') ||
        (c=='!' && d=='=') ||

        (c=='&' && d=='=') ||
        (c=='|' && d=='=') ||
        (c=='^' && d=='=') ||

        (c=='+' && d=='=') ||
        (c=='-' && d=='=') ||
        (c=='*' && d=='=') ||
        (c=='/' && d=='=') ||
        (c=='%' && d=='=') ||

        (c=='+' && d=='+') ||
        (c=='-' && d=='-') ||

        (c=='-' && d=='>') ||

        (c=='>' && d=='>') ||
        (c=='<' && d=='<') ||

        (c=='&' && d=='&') ||
        (c=='|' && d=='|') ||
        0
    );
}

uint tokenizeLine(const FileLine* fl, Token tokens[CASPIAN_MAX_TOKENS_IN_LINE]) {
    const char* text = fl->text;
    const uint  len  = strlen(text);

    uint num_tokens = 0;
    char buf[CASPIAN_MAX_TOKEN_SZ];
    uint buf_index = 0;

    #define appendChar(CHAR) buf[buf_index++]=CHAR
    #define appendToken(TOKEN, INDEX) {\
        if (buf_index > 0) {\
            TOKEN[buf_index] = 0;\
            tokens[num_tokens++] = newToken(INDEX - strlen(TOKEN) + 1, fl, TOKEN);\
        }\
        buf_index = 0;\
    }

    bool in_string = false, in_char = false;

    uint i;
    for (i = 0; i<len; i++) {
        const char c = text[i+0];
        const char d = text[i+1]; /* No error checking here b/c null-terminated guarantees len is always 1 less than actual length of array */

        if ( (c=='/') && (d=='/') ) break; /* Ignore inline comments */

        #define containedCheck(CHAR, FLAG, OTHER_FLAG) {\
            if (!OTHER_FLAG && c==CHAR) {\
                if (FLAG) {\
                    appendChar(c);\
                    appendToken(buf, i+1);\
                    FLAG = false;\
                    continue;\
                } else {\
                    appendToken(buf, i);\
                    appendChar(c);\
                    FLAG = true;\
                    continue;\
                }\
            }\
        }
        containedCheck('\"', in_string, in_char  );
        containedCheck('\'', in_char  , in_string);

        if (in_string || in_char) {
            if (c == '\\' && isValidEscape(d)) {
                appendChar(c);
                appendChar(d);
                i++;
                continue;
            }
            goto APPEND;
        }

        if (isOperator(c, d)) {
            appendToken(buf, i);
            appendChar(c);
            appendChar(d);
            appendToken(buf, i+2);
            i++;
            continue;

        }
        if (isDelimiter(c)) {
            appendToken(buf, i);
            appendChar(c);
            appendToken(buf, i+1);
            continue;
        }

        if (isWhitespace(c)) {
            appendToken(buf, i);
            continue;
        }
APPEND:
        appendChar(c);
    } appendToken(buf, i);
    return num_tokens;
}