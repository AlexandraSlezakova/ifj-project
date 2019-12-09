/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file scanner.c
 * @brief implementation of scanner
 * @author xhladi23 Martin Hladis
 * @author xprasi06 Matej Prasil
 * @author xvadur04 Martin Vadura
 * @author xsleza20 Alexandra Slezakova
 */


#include "scanner.h"

char* keywords[7] = { "def", "else", "if", "None", "pass", "return", "while" };

int is_keyword(char* string)
{
    for (int j = 0; j < 7; j++) {
        if (strcmp(string, keywords[j]) == 0) {
            return j + 4;
        }
    }

    return 0;
}


void create_token(int character, char* string, struct TToken* new_token, TType type)
{

    TType keyword_type;
    /* return read character for another reading */
    ungetc(character, stdin);
    buffer[iterator - 1] = '\0';

    new_token->type = (keyword_type = (TType)is_keyword(string))
                      ? keyword_type
                      : type;


    if (new_token->type == T_INT) {
        char* ptr = malloc(sizeof(char));
        new_token->value.is_int = (int)strtol(string, &ptr, 10);

    }
    else if (new_token->type == T_FLOAT) {
        new_token->value.is_float = strtof(string, NULL);

    }
    else if (new_token->type == T_VAR || new_token->type == T_STRING) {
        new_token->value.is_char = string;
    }
}

int get_token()
{
    int allocated = 0;
    
    iterator = 0;
    buffer = NULL;
    TState state = START; /* initial state */

    indent_counter = 0;

    while (1) {
        /* buffer reallocation */
        buffer = realloc(buffer, (size_t) ++allocated);

        if (state != S_ERROR && state != S_INDENT_ERROR) {
            c = getchar();

            /* write to buffer without comments */
            if (state != S_DOC_CONTENT && state != S_LINE_COMMENT) {
                if (c == 32) {
                    /* make buffer bigger */
                    buffer = realloc(buffer, (size_t) ++allocated);
                    if (c == 32) {
                        strcat(buffer, "\032");
                        iterator++;
                    }
                } else {
                    buffer[iterator++] = c;
                }
            }
            else if (c == 34 && previous_state != START) {
                buffer[iterator++] = c;
            }

        }
        else if (state == S_ERROR) {
            token.type = T_IS_ERR;
            line_position++;
            line_counter++;
            return TOKEN_ERR;

        } else {
            token.type = T_IS_ERR;
            return SYNTAX_ERR;
        }

        switch (state) {
            case START:
                /* identifier */
                if (isalpha(c) || (c == '_')) {
                    state = S_VAR;
                    break;
                }  /* number */
                else if (isdigit(c)) {
                    if (c == '0')
                        state = S_ZERO;
                    else
                        state = S_INT;
                    break;
                } /* white-space character */
                else if (c == '\v' || c == '\f' || c == '\r') {
                    state = START;
                    iterator = 0;
                    allocated = 0;
                    indent_counter = 0;
                    break;
                } /* new line */
                else if (c == '\n') {
                    line_counter++;
                    token.type = T_IS_EOL; /* end of line */
                    previous_state = START;
                    line_position = 0;
                    eol_flag = 1;
                    return 0;
                } /* beginning of string */
                else if (c == 39) {
                    state = START_STRING;
                    break;
                }
                    /* documentation string */
                else if (c == 34) {
                    state = S_START_DOC_1;
                    break;
                } /* line comment */
                else if (c == '#') {
                    state = S_LINE_COMMENT;
                    break;
                } /* equal or assign */
                else if (c == '=') {
                    state = S_ASSIGN;
                    break;
                }  /* mathematical operations */
                else if (c == '+' || c == '-' || c == '*' || c == '/') {
                    state = S_MATH_OP;
                    break;
                }
                else if (c == '<') {
                    state = S_SMALLER;
                    break;
                }
                else if (c == '>') {
                    state = S_GREATER;
                    break;
                }
                else if (c == '!') {
                    state = S_IS_NOT_EQUAL;
                    break;
                }
                else if (c == ',') {
                    state = S_IS_COMMA;
                    break;
                }
                else if (c == '(') {
                    state = S_LEFT_BRACKET;
                    break;
                }
                else if (c == ')') {
                    state = S_RIGHT_BRACKET;
                    break;
                }
                else if (c == ':') {
                    state = S_COLON;
                    break;
                }
                else if (c == ' ') {
                    if (eol_flag) {
                        do {
                            c = getchar();
                            indent_counter++;
                        } while (c == 32);  /* 32 - space */
                        ungetc(c, stdin);
                    }
                    else if (previous_state == START) {
                        state = S_INDENT_ERROR;
                        break;
                    }
                    state = START;
                    iterator = 0;
                    allocated = 0;
                    break;
                }
                else if (c == '\t') {
                    if (eol_flag) {
                        do {
                            c = getchar();
                            indent_counter += 4;
                        } while (c == 9);  /* 9 - TAB */
                        ungetc(c, stdin);
                    }
                    else if (previous_state == START) {
                        state = S_INDENT_ERROR;
                        break;
                    }
                    state = START;
                    iterator = 0;
                    allocated = 0;
                    break;
                }
                else if (c == EOF) {
                    token.type = T_IS_EOF;
                    return TOKEN_OK;
                }
                else {
                    state = S_ERROR;
                    break;
                }


            case S_ZERO:
                if (isdigit(c) || isalpha(c)) {
                    state = S_ERROR;
                    break;
                }
                else {
                    previous_state = S_ZERO;
                    if (c == '.')
                    {
                        state = S_DEC_SEP;
                        break;
                    }
                    line_position++;
                    create_token(c, buffer, &token, T_INT);
                    return 0;
                }

            case S_COLON:
                previous_state = S_COLON;
                line_position++;
                create_token(c, buffer, &token, T_IS_COLON);
                return 0;

                /* numbers */
            case S_INT:
                if (isdigit(c)) {
                    state = S_INT;
                    break;
                }
                else if (c == '.') {
                    state = S_DEC_SEP;
                    break;
                }
                else if (c == 'e' || c == 'E') {
                    state = S_EXP;
                    break;
                }
                else {
                    previous_state = S_INT;
                    line_position++;
                    create_token(c, buffer, &token, T_INT);
                    return 0;
                }

                /* decimal separator */
            case S_DEC_SEP:
                if (isdigit(c)) {
                    state = S_FLOAT;
                }
                else {
                    state = S_ERROR;
                }
                break;

                /* fractional part of number */
            case S_FLOAT:
                if (isdigit(c)) {
                    state = S_FLOAT;
                    break;
                }
                else if (c == 'e' || c == 'E') {
                    state = S_EXP;
                    break;
                }
                else {
                    previous_state = S_FLOAT;
                    line_position++;
                    create_token(c, buffer, &token, T_FLOAT);
                    return 0;
                }

                /* E || e */
            case S_EXP:
                if (c == '+' || c == '-') {
                    state = S_EXP_CHAR;
                    break;
                }
                else if (isdigit(c)) {
                    state = S_NUMBER;
                    break;
                }
                else {
                    previous_state = S_EXP;
                    line_position++;
                    create_token(c, buffer, &token, T_FLOAT);
                    return 0;
                }

                /* character + or - */
            case S_EXP_CHAR:
                if (isdigit(c)) {
                    state = S_NUMBER;
                }
                else {
                    state = S_ERROR;
                }
                break;

                /* exponent number */
            case S_NUMBER:
                if (isdigit(c)) {
                    state = S_NUMBER;
                    break;
                }
                else {
                    previous_state = S_NUMBER;
                    line_position++;
                    create_token(c, buffer, &token, T_FLOAT);
                    return 0;
                }

                /* identifier */
            case S_VAR:
                if (isdigit(c) || isalpha(c) || c == '_') {
                    state = S_VAR;
                    break;
                }
                else {
                    previous_state = S_VAR;
                    line_position++;
                    create_token(c, buffer, &token, T_VAR);
                    return 0;
                }

            case START_STRING:
                if (c == '\\') {
                    state = S_ESC;
                }
                else if (c > 31 && c != 92) {
                    /* 34 - " 92 - \ */
                    if (c == 34 && previous_state == S_DOC_STRING) {
                        state = S_END_DOC_1;
                    }
                    else {
                        state = S_STRING_CONTENT;
                    }
                }
                else if (c == 39) {
                    state = S_STRING;
                }
                else {
                    state = S_ERROR;
                }
                break;

            case S_STRING_CONTENT:
                if (c > 31 && c != 92 && c != 39) {
                    if (c == 34 && previous_state == S_DOC_STRING) {
                        state = S_END_DOC_1;
                    }
                    else {
                        state = S_STRING_CONTENT;
                    }
                }
                else if (c == 39) {
                    state = S_STRING;
                }
                else if (c == '\\') {
                    state = S_ESC;
                }
                else if (c == '\n' && previous_state == S_DOC_STRING) {
                    state = S_STRING_CONTENT;
                }
                else {
                    state = S_ERROR;
                }
                break;

                /* escape sequence */
            case S_ESC:
                if (c == '"' || c == 'n' || c == 't' || c == '\\' || c == 39) {
                    state = S_STRING_CONTENT;
                }
                else if (c == 39) {
                    state = S_STRING;
                }
                else if (c == 'x') {
                    state = S_HEX_ESCAPE;
                }
                else if (isalpha(c)) {
                    state = S_STRING_CONTENT;
                }
                else {
                    state = S_ERROR;
                }
                break;

                /* hex sequence */
            case S_HEX_ESCAPE: //
                if (isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
                    /* \xh */
                    state = S_HEX_ESCAPE2;
                }
                else if (c == 39) {
                    state = S_STRING;
                }
                else {
                    state = S_ERROR;
                }
                break;

            case S_HEX_ESCAPE2:
                if (isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
                    /* \xhh */
                    state = S_STRING_CONTENT;
                    break;
                }
                else if (c == '\0' || (c > 31 && c != 39)) {
                    state = S_STRING_CONTENT;
                }
                else if (c == 39) {
                    state = S_STRING;
                }
                else if (c == '\\') {
                    state = S_ESC;
                }
                else {
                    state = S_ERROR;
                }
                break;

            case S_STRING:
                if (previous_state != S_DOC_STRING) {
                    previous_state = S_STRING;
                    line_position++;
                    create_token(c, buffer, &token, T_STRING);
                    return 0;
                }
                else {
                    state = S_ERROR;
                    break;
                }


            case S_SMALLER:
                if (c == '=') {
                    state = S_IS_SMALLER_OR_EQUAL;
                    break;
                }
                else {
                    previous_state = S_SMALLER;
                    line_position++;
                    create_token(c, buffer, &token, T_IS_SMALLER);
                    return 0;
                }

            case S_IS_SMALLER_OR_EQUAL:
                previous_state = S_IS_SMALLER_OR_EQUAL;
                line_position++;
                create_token(c, buffer, &token, T_IS_SMALLER_OR_EQUAL);
                return 0;

            case S_GREATER:
                if (c == '=') {
                    state = S_IS_GREATER_OR_EQUAL;
                    break;
                }
                else {
                    previous_state = S_GREATER;
                    line_position++;
                    create_token(c, buffer, &token, T_IS_GREATER);
                    return 0;
                }

            case S_IS_GREATER_OR_EQUAL:
                previous_state = S_IS_GREATER_OR_EQUAL;
                line_position++;
                create_token(c, buffer, &token, T_IS_GREATER_OR_EQUAL);
                return 0;

            case S_ASSIGN:
                if (c == '=') {
                    state = S_IS_EQUAL;
                    break;
                }
                else {
                    previous_state = S_ASSIGN;
                    line_position++;
                    create_token(c, buffer, &token, T_ASSIGNMENT);
                    return 0;
                }

            case S_IS_EQUAL:
                previous_state = S_IS_EQUAL;
                line_position++;
                create_token(c, buffer, &token, T_IS_EQUAL);
                return 0;

                /* mathematical operation */
            case S_MATH_OP:
                if (buffer[iterator - 2] == '+') {
                    previous_state = S_MATH_OP;
                    line_position++;
                    create_token(c, buffer, &token, T_ADD);
                    return 0;
                }
                else if (buffer[iterator - 2] == '-') {
                    previous_state = S_MATH_OP;
                    line_position++;
                    create_token(c, buffer, &token, T_SUB);
                    return 0;
                }
                else if (buffer[iterator - 2] == '*') {
                    previous_state = S_MATH_OP;
                    line_position++;
                    create_token(c, buffer, &token, T_MUL);
                    return 0;
                }
                else if (buffer[iterator - 2] == '/') {
                    if (buffer[iterator - 1] == '/') {
                        state = S_DIV_INT;
                        break;
                    }
                    else {
                        previous_state = S_MATH_OP;
                        line_position++;
                        create_token(c, buffer, &token, T_DIV);
                        return 0;
                    }

                }
                else {
                    state = S_ERROR;
                    break;
                }

            case S_DIV_INT:
                previous_state = S_DIV_INT;
                line_position++;
                create_token(c, buffer, &token, T_DIV_INT);
                return 0;

                /* documentation string */
            case S_START_DOC_1:
                if (c == 34) {
                    state = S_START_DOC_2;
                    if (previous_state != START) previous_state = S_DOC_STRING;
                }
                else {
                    state = S_ERROR;
                }
                break;

            case S_START_DOC_2:
                if (c == 34) {
                    if (previous_state != START) {
                        previous_state = S_DOC_STRING;
                        state = S_STRING_CONTENT;
                    }
                    else {
                        state = S_DOC_CONTENT;
                    }
                }
                else {
                    state = S_ERROR;
                }
                break;

            case S_DOC_CONTENT:
                if (c != 34) {
                    state = S_DOC_CONTENT;
                }
                else {
                    state = S_END_DOC_1;
                }
                break;

            case S_END_DOC_1:
                if (c == 34) {
                    state = S_END_DOC_2;
                }
                else {
                    state = S_ERROR;
                }
                break;

            case S_END_DOC_2:
                if (c == 34) {
                    if (previous_state == S_DOC_STRING) {
                        c = getchar();  /* get next character to return */
                        create_token(c, buffer, &token, T_STRING);
                        return 0;
                    }
                    else {
                        state = S_END_DOC_2;
                        break;
                    }
                }
                else if (c == '\n') {
                    state = START;
                    allocated = 0;
                    iterator = 0;
                    line_position = 0;
                    line_counter++;
                    buffer = NULL;
                }
                else {
                    state = S_ERROR;
                }

                previous_state = START;
                break;


                /* no equal */
            case S_IS_NOT_EQUAL:
                if (c == '=') {
                    c = getchar();
                    buffer[iterator++] = c;
                    previous_state = S_IS_NOT_EQUAL;
                    line_position++;
                    create_token(c, buffer, &token, T_IS_NOT_EQUAL);
                    return 0;
                }
                else {
                    state = S_ERROR;
                    break;
                }

            case S_LINE_COMMENT:
                if (c == '\n') {
                    if (previous_state != START) {
                        state = START;
                        ungetc('\n', stdin);
                    }
                    else {
                        state = START;
                    }
                    indent_counter = 0;
                }
                else {
                    state = S_LINE_COMMENT;
                }
                iterator = 0;
                allocated = 0;
                break;

            case S_IS_COMMA:
                previous_state = S_IS_COMMA;
                line_position++;
                create_token(c, buffer, &token, T_IS_COMMA);
                return 0;

            case S_LEFT_BRACKET:
                previous_state = S_LEFT_BRACKET;
                line_position++;
                create_token(c, buffer, &token, T_LEFT_BRACKET);
                return 0;

            case S_RIGHT_BRACKET:
                previous_state = S_RIGHT_BRACKET;
                line_position++;
                create_token(c, buffer, &token, T_RIGHT_BRACKET);
                return 0;

            default:
                state = S_ERROR;
                break;

        }
    }
}

void unget_token()
{

    int i = 0;
    unsigned long length = strlen(buffer);

    while (buffer[i] != '\0') {
        ungetc(buffer[--length], stdin);
        i++;
    }
}