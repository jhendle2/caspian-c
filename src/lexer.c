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
    fl.text[CASPIAN_MAX_FILELINE_SZ-1] = 0;
    return fl;
}

void printFileLine(const FileLine* fl) {
    printf(BGREEN "%s:%u: " RESET "%s\n", fl->origin, fl->line_number, fl->text);
}

static bool isPreprocessorLine(const char* s) {
    const char* t = s;
    while (*t) {
        if (*t == ' ' || *t == '\t') t++;
        else if (*t == '#') return true;
        else break;
    } return false;
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
        buf[CASPIAN_MAX_FILELINE_SZ-1] = 0;
        line_number++;                    /* Every line number is tracked, even blank ones */

        char* stripped = buf;
        // char* stripped = lstrip(buf);  /* Drop the leading spaces so we can print properly later */
        replace(stripped, '\n', '\0');    /* Drop the newline at the end of the fileline for printing */
        if (empty(stripped)) continue;    /* Ignore blank lines */
        if (!CASPIAN_DISABLE_PREPROCESSOR && isPreprocessorLine(stripped)) continue;

        const FileLine read_line = newFileLine(line_number, file_path, stripped);
        // printFileLine(&read_line);

        if (num_lines_in_file >= CASPIAN_MAX_LINES_IN_FILE-1) {
            error_line(1, read_line, "Too many lines in file. Maximum is currently set to [%u].", CASPIAN_MAX_LINES_IN_FILE);
        }
        file_as_lines[num_lines_in_file++] = read_line;
    }

    fclose(fp);
    return num_lines_in_file;
}

/****************************************************************************************************/
Token newToken(const uint offset, const FileLine* origin, const char* text) {
    Token token = { .offset=offset, .origin=*origin};
    strncpy(token.text, text, CASPIAN_MAX_TOKEN_SZ);
    token.text[CASPIAN_MAX_TOKEN_SZ-1] = 0;
    return token;
}

#define CASPIAN_MAX_TOKEN_HEADER_SZ (CASPIAN_MAX_PATH_SZ + 1 + 5 + 1 + 5 + 2) /* PATH + COLON + 5-digits + COLON + 5-digits + COLON + SPACE */
void printToken(const Token* token) {
    const FileLine fl = token->origin;
    char temp[CASPIAN_MAX_TOKEN_HEADER_SZ] = "";
        snprintf(temp, CASPIAN_MAX_TOKEN_HEADER_SZ, "%s:%u:%u: ", fl.origin, fl.line_number, token->offset);
    const uint header_len = strlen(temp), token_len = strlen(token->text);
        printf(BGREEN "%s" RESET "%s\n", temp, fl.text);
        printf(BGREEN "%*c", header_len + token->offset - 1, ' '); /* Minus 1 because offset is counted started at 1, not 0 */
    for (uint i = 0; i<token_len; i++)
        printf("^");
    printf(RESET "\n");
}

bool cmpToken(const Token* token, const char* str) {
    return (strncmp(token->text, str, CASPIAN_MAX_TOKEN_SZ)==0);
}

bool cmpTokens(const Token* token, const Token* other) {
    return (strncmp(token->text, other->text, CASPIAN_MAX_TOKEN_SZ)==0);
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
        case '\\':
            return true;
        default: return false;
    }
}

static inline bool isOperator2(const char c, const char d) {
    /* This chaining should be faster than `strncmp` for each case */
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

        /* Block comments are considered "operators"
            for simplicity in the Syntax tree builder */
        (c=='/' && d=='*') ||
        (c=='*' && d=='/')
    );
}
static inline bool isOperator3(const char c, const char d, const char e) {
    return (
        (c=='<' && d=='<' && e=='=') ||
        (c=='>' && d=='>' && e=='=')
    );
}

uint tokenizeLine(const FileLine* fl, Token tokens[CASPIAN_MAX_TOKENS_IN_LINE]) {
    const char* text = fl->text;
    const uint  len  = strlen(text);

    uint num_tokens = 0, buf_index = 0;
    char buf[CASPIAN_MAX_TOKEN_SZ];
    bool in_string = false, in_char = false;

    /* Lambda equivalents for pushing char and pushing a token */
    #define appendChar(CHAR) {\
        if (buf_index == CASPIAN_MAX_TOKEN_SZ-1) {\
            error_line(1, *fl, "Token is too long, maximum length is (%u).", CASPIAN_MAX_TOKEN_SZ);\
        }\
        buf[buf_index++]=CHAR;\
    }
    #define appendToken(TOKEN, INDEX) {\
        if (buf_index > 0) {\
            if (num_tokens == CASPIAN_MAX_TOKENS_IN_LINE-1) {\
                error_line(1, *fl, "Too many tokens in line, maximum number of tokens is (%u).", CASPIAN_MAX_TOKENS_IN_LINE);\
            }\
            TOKEN[buf_index] = 0;\
            tokens[num_tokens++] = newToken(INDEX - strlen(TOKEN) + 1, fl, TOKEN);\
        }\
        buf_index = 0;\
    }

    /* Lambda equivalent for single & double quoted groups */
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

    uint i;
    for (i = 0; i<len; i++) {
        const char c =     text[i+0];
        const char d =     text[i+1];     /* No error checking here b/c null-terminated guarantees 
                                             len is always 1 less than actual length of array */
        const char e = d ? text[i+2] : 0; /* Error checking if `d` is null character */
        
        if ( (c=='/') && (d=='/') ) break;       /* Ignore inline comments */

        /**************************************************************************/
        /* String constants & Characters ******************************************/
        
        containedCheck('\"', in_string, in_char  );
        containedCheck('\'', in_char  , in_string);

        if (in_string || in_char) {
            if (c == '\\' && isValidEscape(d)) {
                appendChar(c);
                appendChar(d);
                i++;
                continue;
            }
            appendChar(c);
            continue;
        }

        /**************************************************************************/
        /* Operators and delimiters ***********************************************/

        if (isOperator3(c, d, e)) {
            appendToken(buf, i);
            appendChar (c); appendChar (d); appendChar (e);
            appendToken(buf, i+3); /* And skip 3 so we include throth operators */
            i+=2;
            continue;

        }
        if (isOperator2(c, d)) {
            appendToken(buf, i);
            appendChar (c); appendChar (d);
            appendToken(buf, i+2); /* And skip 2 so we include both operators */
            i++;
            continue;

        }

        if (c == '.' && buf_index>0 && isDec(buf[buf_index-1])) {
            appendChar (c);
            continue;
        }

        if (isDelimiter(c)) {
            appendToken(buf, i);
            appendChar (c);
            appendToken(buf, i+1); /* And skip 1 so we include the delimiter */
            continue;
        }

        if (isWhitespace(c)) {
            appendToken(buf, i);
            continue;
        }

        /**************************************************************************/
        /* Regular Characters *****************************************************/
        if (c == '\\') continue; // TODO: Remove b/c preprocessor would have taken care of these. Here right now for testing
        appendChar(c);

    } appendToken(buf, i); /* Clear out the buffer at the end */
    return num_tokens;
}

#define BRIEF 25
void printTokensNoNewline(const Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], const uint len) {
    uint i;
    for (i = 0; i<len && i<BRIEF; i++)
        printf("`%s` ", tokens[i].text);
    if (i>BRIEF) printf("... `%s`", tokens[len-1].text);
}
void printTokens(const Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], const uint len) {
    printTokensNoNewline(tokens, len);
    printf("\n");
}

uint copyTokens(Token a[CASPIAN_MAX_TOKENS_IN_LINE], const Token b[CASPIAN_MAX_TOKENS_IN_LINE], const uint len) {
    for (uint i = 0; i<len; i++)
        a[i] = b[i];
    return len;
}

uint moveTokens(Token a[CASPIAN_MAX_TOKENS_IN_LINE], Token b[CASPIAN_MAX_TOKENS_IN_LINE], const uint len) {
    memmove(a, b, len * sizeof(Token));
    return len;
}

uint appendTokens(Token a[CASPIAN_MAX_TOKENS_IN_LINE], const uint a_len, const Token b[CASPIAN_MAX_TOKENS_IN_LINE], const uint b_len) {
    uint len = (a_len + b_len) % CASPIAN_MAX_TOKENS_IN_LINE;
    for (uint a_i = a_len, b_i = 0; a_i<len; a_i++, b_i++)
        a[a_i] = b[b_i];
    return len;
}

void splitTokens(Token input[CASPIAN_MAX_TOKENS_IN_LINE], const uint input_len, const uint split_index,
                 Token left[CASPIAN_MAX_TOKENS_IN_LINE] , uint* left_len,
                 Token right[CASPIAN_MAX_TOKENS_IN_LINE], uint* right_len) {
    *left_len  = split_index + 1;
    *right_len = input_len - split_index - 1;    
    moveTokens(left, input, *left_len);
    moveTokens(right, input + (*left_len), (*right_len) );
}

Token popFrontTokens(Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], uint* num_tokens) {
    Token popped = tokens[0];
    Token temp[CASPIAN_MAX_TOKENS_IN_LINE];
    (*num_tokens)--;
    memmove(temp  , tokens+1, sizeof(Token)*(*num_tokens));
    memmove(tokens, temp    , sizeof(Token)*(*num_tokens));
    return popped;
}

int findToken(Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], const uint num_tokens, const char* find) {
    for (uint i = 0; i<num_tokens; i++) {
        if (cmpToken( &(tokens[i]), find )) return i;
    }
    return NOT_FOUND;
}

uint pushBackTokens(Token tokens[CASPIAN_MAX_TOKENS_IN_LINE], uint num_tokens, const Token* push_token) {
    num_tokens++;
    if (num_tokens == CASPIAN_MAX_TOKENS_IN_LINE) {
        error_token(1, *push_token, "Too many tokens in a single statement");
    }
    tokens[num_tokens-1] = (*push_token);
    return num_tokens;
}

bool isOperatorDelimiter(const Token* token) {
    return (
        (strlen(token->text)==1 && isDelimiter(token->text[0])) ||
        (strlen(token->text)==2 && isOperator2(token->text[0], token->text[1])) ||
        (strlen(token->text)==3 && isOperator3(token->text[0], token->text[1], token->text[2]))
    ); 
}