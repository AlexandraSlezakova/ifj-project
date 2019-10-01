/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file scanner.c
 * @brief implementation of scanner
 * @author
 * @author
 * @author
 * @author
 */


#include "scanner.h"

char *keywords[7] = {"def", "else", "if", "None", "pass", "return", "while"};

int is_keyword(char *string)
{
    for (int j = 0; j < 7; j++) {
        if (strcmp(string, keywords[j]) == 0) {
            return j + 4;
        }
    }

    return 0;
}


void create_token(int character, char *string, struct TToken *new_token, TType type) {

    TType keyword_type;
    /* return read character for another reading */
    ungetc(character, stdin);
    buffer[iterator - 1] = '\0';

    new_token->type = (keyword_type = (TType)is_keyword(string))
                      ? keyword_type
                      : type;


    if (new_token->type == T_INT) {
        char *ptr = malloc(sizeof(char));
        new_token->value.is_int = (int)strtol(string, &ptr, 10);

    } else if (new_token->type  == T_FLOAT) {
        new_token->value.is_float = strtof(string, NULL);

    } else if (new_token->type  == T_VAR || new_token->type == T_STRING) {
        new_token->value.is_char = string;
    }
}


int get_token() {

    static int line_counter = 0; /* number of lines */
    int line_position = 0;       /* position */
    int allocated = 0;

    iterator = 0;
    buffer = NULL;
    TState state = START; /* initial state */

    indent_counter = 0;

    while (1) {
        /* buffer reallocation */
        buffer = realloc(buffer, (size_t) ++allocated);

        if (state != S_ERROR) {
            if ((c = getchar()) == EOF && state == START) {
                token.type = T_IS_EOF;
                return TOKEN_OK;
            }

            line_position++;

            /* write to buffer without comments */
            if (((c != 34) && state != S_DOC_CONTENT) && (c != 35 && state != S_LINE_COMMENT)) {
                buffer[iterator++] = c;
            }

        } else {
            token.type = T_IS_ERR;
            fprintf(stderr, "Wrong token - line:%d (character:%d)!\n", line_counter, line_position);
            return TOKEN_ERR;
        }

        switch (state) {
            case START:
                /* identifier */
                if (isalpha(c) || (c == '_')) {
                    state = S_VAR;
                    break;
                }  /* number */
                else if (isdigit(c)) {
                    if(c == '0')
                        state = S_ZERO;
                    else
                        state = S_INT;
                    break;
                } /* white-space character */ // todo
                else if (c == '\v' || c == '\f' || c == '\r' || c == '\t') {
                    state = START;
                    iterator = 0;
                    allocated = 0;
                    break;
                } /* new line */
                else if (c == '\n') {
                    line_counter++;
                    token.type = T_IS_EOL; /* end of line */
                    previous_state = S_IS_EOL;
                    return 0;
                } /* beginning of string */
                else if (c == 39) {
                    state = START_STRING;
                    break;
                } /* documentation string */
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
                    if (previous_state == S_IS_EOL) {
                        do {
                            c = getchar();
                            indent_counter++;
                        } while (c == 32);  /* 32 - space */
                        ungetc(c, stdin);
                    } else if (previous_state == START) {
                        state = S_ERROR;
                        break;
                    }
                    state = START;
                    iterator = 0;
                    allocated = 0;
                    break;
                }
                else {
                    state = S_ERROR;
                    break;
                }


            case S_ZERO:
                if(isdigit(c) || isalpha(c)) {
                    state = S_ERROR;
                    break;
                }
                else {
                    previous_state =  S_ZERO;
                    create_token(c, buffer, &token, T_INT);
                    return 0;
                }

            case S_COLON:
                previous_state = S_COLON;
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
                    create_token(c, buffer, &token, T_VAR);
                    return 0;
                }

            case START_STRING:
                if (c == '\\') {
                    state = S_ESC;
                }
                else if (c > 31 && c != 34 && c != 92) {
                    /* 34 - " 92 - \ */
                    state = S_STRING_CONTENT;
                }
                else if (c == 39) {
                    state = S_STRING;
                }
                else {
                    state = S_ERROR;
                }
                break;

            case S_STRING_CONTENT:
                if (c > 31 && c != 34 && c != 92 && c != 39) {
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

                /* escape sequence */
            case S_ESC:
                if (c == '"' || c == 'n' || c == 't' || c == '\\') {
                    state = S_STRING_CONTENT;
                }
                else if (c == 39) {
                    state = S_STRING;
                }
                else if (c == 'x') {
                    state = S_HEX_ESCAPE;
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
                previous_state = S_STRING;
                create_token(c, buffer, &token, T_STRING);
                return 0;

            case S_SMALLER:
                if (c == '=') {
                    state = S_IS_SMALLER_OR_EQUAL;
                    break;
                }
                else {
                    previous_state = S_SMALLER;
                    create_token(c, buffer, &token, T_IS_SMALLER);
                    return 0;
                }

            case S_IS_SMALLER_OR_EQUAL:
                previous_state = S_IS_SMALLER_OR_EQUAL;
                create_token(c, buffer, &token, T_IS_SMALLER_OR_EQUAL);
                return 0;

            case S_GREATER:
                if (c == '=') {
                    state = S_IS_GREATER_OR_EQUAL;
                    break;
                }
                else {
                    previous_state = S_GREATER;
                    create_token(c, buffer, &token, T_IS_GREATER);
                    return 0;
                }

            case S_IS_GREATER_OR_EQUAL:
                previous_state = S_IS_GREATER_OR_EQUAL;
                create_token(c, buffer, &token, T_IS_GREATER_OR_EQUAL);
                return 0;

            case S_ASSIGN:
                if (c == '=') {
                    state = S_IS_EQUAL;
                    break;
                } else {
                    previous_state = S_ASSIGN;
                    create_token(c, buffer, &token, T_ASSIGNMENT);
                    return 0;
                }

            case S_IS_EQUAL:
                previous_state = S_IS_EQUAL;
                create_token(c, buffer, &token, T_IS_EQUAL);
                return 0;

            /* mathematical operation */
            case S_MATH_OP:
                if (buffer[iterator - 2] == '+') {
                    previous_state = S_MATH_OP;
                    create_token(c, buffer, &token, T_ADD);
                    return 0;
                }
                else if (buffer[iterator - 2] == '-') {
                    previous_state = S_MATH_OP;
                    create_token(c, buffer, &token, T_SUB);
                    return 0;
                }
                else if (buffer[iterator - 2] == '*') {
                    previous_state = S_MATH_OP;
                    create_token(c, buffer, &token, T_MUL);
                    return 0;
                }
                else if (buffer[iterator - 2] == '/') {
                    if (buffer[iterator - 1] == '/') {
                        state = S_DIV_INT;
                        break;
                    } else {
                        previous_state = S_MATH_OP;
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
                create_token(c, buffer, &token, T_DIV_INT);
                return 0;

            /* documentation string */
            case S_START_DOC_1:
                if (c == 34) {
                    state = S_START_DOC_2;
                } else {
                    state = S_ERROR;
                }
                break;

            case S_START_DOC_2:
                if (c == 34) {
                    state = S_DOC_CONTENT;
                } else {
                    state = S_ERROR;
                }
                break;

            case S_DOC_CONTENT:
                if (c != 34) {
                    state = S_DOC_CONTENT;
                } else {
                    state = S_END_DOC_1;
                }
                break;

            case S_END_DOC_1:
                if (c == 34) {
                    state = S_END_DOC_2;
                } else {
                    state = S_ERROR;
                }
                break;

            case S_END_DOC_2:
                if (c == 34) {
                    state = S_END_DOC_2;
                } else if (c == '\n') {
                    state = START;
                    allocated = 0;
                    iterator = 0;
                    line_position = 0;
                    line_counter++;
                } else {
                    state = S_ERROR;
                }
                previous_state = S_END_DOC_2;
                break;

            /* no equal */
            case S_IS_NOT_EQUAL:
                if (c == '=') {
                    c = getchar();
                    buffer[iterator++] = c;
                    previous_state = S_IS_NOT_EQUAL;
                    create_token(c, buffer, &token, T_IS_NOT_EQUAL);
                    return 0;
                } else {
                    state = S_ERROR;
                    break;
                }

            case S_LINE_COMMENT:
                if (c == '\n') {
                    state = START;
                }
                else {
                    state = S_LINE_COMMENT;
                }
                break;

            case S_IS_COMMA:
                previous_state = S_IS_COMMA;
                create_token(c, buffer, &token, T_IS_COMMA);
                return 0;

            case S_LEFT_BRACKET:
                previous_state = S_LEFT_BRACKET;
                create_token(c, buffer, &token, T_LEFT_BRACKET);
                return 0;

            case S_RIGHT_BRACKET:
                previous_state = S_RIGHT_BRACKET;
                create_token(c, buffer, &token, T_RIGHT_BRACKET);
                return 0;

            default:
                state = S_ERROR;
                break;

        }
    }
}