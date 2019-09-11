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

#include <string.h>
#include <ctype.h>
#include <memory.h>
#include <stdlib.h>
#include <stdbool.h>
#include "scanner.h"
#include "errors.h"

char *keywords[9] = {"def", "do", "while", "if", "then", "elif", "not", "end", "nil"};

bool is_comment_begin()
{
    for (int j = 0; j < 5; ++j) {
        buffer[i++] = getchar();
    }

    return ((!strcmp(buffer, "=begin ")) || (!strcmp(buffer, "=begin\t")) || (!strcmp(buffer, "=begin\n")));

}

bool is_comment_end()
{
    for (int j = 0; j < 3; ++j) {
        buffer[i++] = (char)getchar();
    }

    char *pom_buffer = malloc(5);
    memcpy(pom_buffer, buffer + (i-5), 5);

    return ((!strcmp(pom_buffer, "=end ")) || (!strcmp(pom_buffer, "=end\t")) || (!strcmp(pom_buffer, "=end\n")));

}

int is_keyword(char *string)
{
    for (int j = 0; j < 9; j++) {
        if (strcmp(string, keywords[j]) == 0) {
            return j + 5;
        }
    }

    return 0;

}


void create_token(char character, char *string, struct TToken *new_token, TType type) {

    TType keyword_type;
    /* return read character for another reading */
    ungetc(character, stdin);
    buffer[i - 1] = '\0';

    new_token->type = (keyword_type = (TType)is_keyword(string))
                      ? keyword_type
                      : type;


    if (new_token->type == T_INT) {
        char *ptr = malloc(sizeof(char));
        new_token->value.is_int = strtol(string, &ptr, 10);

    } else if (new_token->type  == T_FLOAT) {
        new_token->value.is_float = strtof(string, NULL);

    } else if (new_token->type  == T_VAR || new_token->type == T_FCE || new_token->type == T_STRING) {
        new_token->value.is_char = string;
    }
}

int get_token() {

    static int lineNum = 0; /* number of line */
    int linePos = 0; /* position */
    int allocated = 0;

    i = 0;
    buffer = NULL;
    TState state = START; /* initial state */

    while (1) {
        /* buffer realloc */
        buffer = realloc(buffer, (size_t) ++allocated);

        if (state != S_ERROR) {
            if ((c = getchar()) == EOF && state == START) {
                token.type = T_IS_EOF;
                return TOKEN_OK;
            }

            linePos++;

            /* write to buffer without comments */
            if (!((state == START_BLOCK_COMMENT || state == END_BLOCK_COMMENT) && c != '=')) {
                buffer[i++] = c; /* save character to buffer */
            }
        } else {
            token.type = T_IS_ERR;
            fprintf(stderr, "Wrong token - line:%d (character:%d)!\n", lineNum, linePos);
            return TOKEN_ERR;
        }

        switch (state) {
            case START:
                /* identifier */
                if ((c>='a' && c<='z') || (c == '_')) {
                    state = S_VAR;
                    break;
                }  /* number */
                else if (isdigit(c)) {
                    if(c == '0')
                        state = S_ZERO;
                    else
                        state = S_INT;
                    break;
                } /* white-space character*/
                else if (isspace(c) && c != '\n') {
                    state = START;
                    i = 0;
                    allocated = 0;
                    break;
                } /* new line*/
                else if (c == '\n') {
                    lineNum++;
                    token.type = T_IS_EOL; /* end of line */
                    return 0;
                } /* beginning of string*/
                else if (c == '"') {
                    state = START_STRING;
                    break;
                } /* line comment*/
                else if (c == '#') {
                    state = S_COMMENT;
                    break;
                } /* block comment, equal or assignment */
                else if (c == '=') {
                    state = S_COMMENT_EQ_ASSIGNMENT;
                    break;
                }  /* mathematical operations */
                else if (c == '+')
                {
                    c = getchar();
                    create_token(c, buffer, &token, T_ADD);
                    return 0;
                }
                else if (c == '-')
                {
                    c = getchar();
                    create_token(c, buffer, &token, T_SUB);
                    return 0;
                }
                else if (c == '*')
                {
                    c = getchar();
                    create_token(c, buffer, &token, T_MUL);
                    return 0;
                }
                else if (c == '/')
                {
                    c = getchar();
                    create_token(c, buffer, &token, T_DIV);
                    return 0;
                }
                else if (c == '<')
                {
                    state = S_SMALLER;
                    break;
                }
                else if (c == '>')
                {
                    state = S_GREATER;
                    break;
                }
                else if (c == '!')
                {
                    state = S_IS_NOT_EQUAL;
                    break;
                }
                else if (c == ',')
                {
                    c = getchar();
                    create_token(c, buffer, &token, T_IS_COMMA);
                    return 0;
                }
                else if (c == '(')
                {
                    c = getchar();
                    create_token(c, buffer, &token, T_LEFT_BRACKET);
                    return 0;
                }
                else if (c == ')')
                {
                    c = getchar();
                    create_token(c, buffer, &token, T_RIGHT_BRACKET);
                    return 0;
                }
                else if (c == ':')
                {
                    c = getchar();
                    create_token(c, buffer, &token, T_COLON);
                    return 0;
                }
                else if (c == '\t')
                {
                    state = S_TAB;
                    break;
                }
                else
                {
                    state = S_ERROR;
                    break;
                }
            case S_ZERO:
                if(isdigit(c))
                    state = S_ERROR;
                else
                {
                    ungetc(c, stdin);
                    state = S_INT;
                }
                break;

                /* NUMBERS */
            case S_INT:
                if (isdigit(c))
                {
                    state = S_INT;
                    break;
                }
                else if (c == '.')
                {
                    state = S_DEC_SEP;
                    break;
                }
                else if (c == 'e' || c == 'E')
                {
                    state = S_EXP;
                    break;
                }
                else if (is_special_character_number())
                {
                    state = S_ERROR;
                    break;
                }
                else
                {
                    create_token(c, buffer, &token, T_INT);
                    return 0;
                }

                /* decimal separator */
            case S_DEC_SEP:
                if (isdigit(c))
                {
                    state = S_FLOAT;
                    break;
                }
                else
                    state = S_ERROR;
                break;

                /* fractional part of number */
            case S_FLOAT:
                if (isdigit(c))
                {
                    state = S_FLOAT;
                    break;
                }
                else if (c == 'e' || c == 'E')
                {
                    state = S_EXP;
                    break;
                }
                else if (is_special_character_number())
                {
                    state = S_ERROR;
                    break;
                }
                else
                {
                    create_token(c, buffer, &token, T_FLOAT);
                    return 0;
                }

                /* E || e */
            case S_EXP:
                if (c == '+' || c == '-')
                {
                    state = S_EXP_CHAR;
                    break;
                }
                else if (isdigit(c))
                {
                    state = S_NUMBER_END;
                    break;
                }
                else {
                    create_token(c, buffer, &token, T_FLOAT);
                    return 0;
                }

                /* character + or - */
            case S_EXP_CHAR:
                if (isdigit(c))
                {
                    state = S_NUMBER_END;
                    break;
                }
                else
                    state = S_ERROR;
                break;

                /* exponent number */
            case S_NUMBER_END:
                if (isdigit(c))
                {
                    state = S_NUMBER_END;
                    break;
                }
                else if (is_special_character_number())
                {
                    state = S_ERROR;
                    break;
                }
                else
                {
                    create_token(c, buffer, &token, T_FLOAT);
                    return 0;
                }

                /* identifier */
            case S_VAR:
                if (isdigit(c) || isalpha(c) || c == '_')
                {
                    state = S_VAR;
                    break;
                } /* function identifier */
                else if (c == '!' || c == '?')
                {
                    c = getchar();
                    create_token(c, buffer, &token, T_FCE);
                    return 0;
                }
                else if (is_special_character()) {
                    state = S_ERROR;
                    break;
                }
                else
                {
                    create_token(c, buffer, &token, T_VAR);
                    return 0;
                }

            case START_STRING:
                if (c == '\\')
                {
                    state = S_ESC;
                    break;
                }
                else if (c > 31 && c != 34 && c != 92)
                { /* 34 - " 92 - \ */
                    state = S_STRING;
                    break;
                }
                else if (c == '"')
                {
                    state = END_STRING;
                    break;
                }
                else
                    state = S_ERROR;
                break;

            case S_STRING:
                if (c > 31 && c != 34 && c != 92)
                {
                    state = S_STRING;
                    break;
                }
                else if (c == '"')
                {
                    state = END_STRING;
                    break;
                }
                else if (c == '\\')
                {
                    state = S_ESC;
                    break;
                }
                else
                    state = S_ERROR;
                break;

                /* escape sequence */
            case S_ESC:
                if (c == '"' || c == 'n' || c == 't' || c == 's' || c == '\\')
                {
                    state = S_STRING;
                    break;
                }
                else if (c == 'x')
                {
                    state = S_HEX_ESCAPE;
                    break;
                }
                else
                    state = S_ERROR;
                break;

                /* hex sequence */
            case S_HEX_ESCAPE: //
                if (isdigit(c) || (c >= 'A' && c <= 'F'))
                {  /* \xh */
                    state = S_HEX_ESCAPE2;
                    break;
                }
                else
                    state = S_ERROR;
                break;

            case S_HEX_ESCAPE2:
                if (isdigit(c) || (c >= 'A' && c <= 'F'))
                {   /* \xhh */
                    state = S_STRING;
                    break;
                }
                else if (c == '\0')
                {
                    state = S_STRING;
                    break;
                }
                else if (c == '"')
                {
                    c = getchar();
                    create_token(c, buffer, &token, T_STRING);
                    return 0;
                }
                else if (c == '\\')
                {
                    state = S_ESC;
                    break;
                }
                else
                    state = S_ERROR;
                break;

            case S_SMALLER:
                if (c == '=')
                {
                    c = getchar();
                    create_token(c, buffer, &token, T_IS_SMALLER_OR_EQUAL);
                    return 0;
                }
                else
                {
                    create_token(c, buffer, &token, T_IS_SMALLER);
                    return 0;
                }


            case S_GREATER:
                if (c == '=')
                {
                    c = getchar();
                    create_token(c, buffer, &token, T_IS_GREATER_OR_EQUAL);
                    return 0;
                }
                else
                {
                    create_token(c, buffer, &token, T_IS_GREATER);
                    return 0;
                }

            case S_COMMENT_EQ_ASSIGNMENT:
                if (c == '=')
                {
                    c = getchar();
                    create_token(c, buffer, &token, T_IS_EQUAL);
                    return 0;
                }
                else if (c == 'b')
                {
                    if (is_comment_begin()) {
                        state = START_BLOCK_COMMENT;
                        linePos += 5;
                    }
                    else
                        state = S_ERROR;
                    break;
                }
                else
                    ungetc(c, stdin);
                state = S_ASSIGNMENT;
                break;

            case START_BLOCK_COMMENT:
                if (c == 'e' && buffer[linePos-2] == '=') // linePos-2 lebo indexace je od 0 ale linePos sa pocita od 1
                {
                    buffer[i++] = c;
                    if (is_comment_end()) {
                        linePos += 3;
                        if (c == '\n') {
                            lineNum++;
                            token.type = T_IS_EOL;
                            return 0;
                        }
                        else state = END_BLOCK_COMMENT;
                    }
                    else
                        state = S_ERROR;
                    break;
                }
                else
                {
                    if (c == '\n'){
                        lineNum++;
                        linePos = 0;
                        i = 0;
                        allocated = 0;
                        token.type = T_IS_EOL;
                    }
                    state = START_BLOCK_COMMENT;
                    break;
                }


            case END_BLOCK_COMMENT:
                if (c == '\n') {
                    lineNum++;
                    token.type = T_IS_EOL;
                    return 0;
                }
                break;


            case S_IS_NOT_EQUAL:
                if (c == '=') {
                    c = getchar();
                    create_token(c, buffer, &token, T_IS_NOT_EQUAL);
                    return 0;
                } else
                    state = S_ERROR;
                break;

            case S_ASSIGNMENT:
                create_token(c, buffer, &token, T_ASSIGNMENT);
                return 0;

            case S_COMMENT:
                if (c == '\n')
                {
                    state = START;
                    break;
                }
                else
                {
                    state = S_COMMENT;
                    break;
                }

            default:
                state = S_ERROR;
                break;

        }
    }
}

bool is_special_character()
{
    /* semicolon, special characters, @ */
    return c == 59
           || c == '.'
           || (c >= 33 && c <= 39)
           || c == 64
           || (c >= 91 && c <= 96)
           || c >= 123;
}

bool is_special_character_number()
{
    /* special characters, comma, hyphen, semicolon */
    return (c >= 33 && c <= 39)
        || c == 44
        || c == 45
        || c == 59
        || c >= 63;
}
