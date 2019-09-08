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

char *keywords[9] = {"def", "do", "else", "end", "if", "not", "nil", "then", "while"};

bool is_comment_begin()
{
    for (int i = 0; i < 5; ++i) {
        buffer[count++] = (char)getchar();
    }

    return ((!strcmp(buffer, "=begin ")) || (!strcmp(buffer, "=begin\t")) || (!strcmp(buffer, "=begin\n")));

}

bool is_comment_end()
{
    for (int i = 0; i < 3; ++i) {
        buffer[count++] = (char)getchar();
    }

    char *pom_buffer = malloc(5);
    memcpy(pom_buffer, buffer + (count-5), 5);

    return ((!strcmp(pom_buffer, "=end ")) || (!strcmp(pom_buffer, "=end\t")) || (!strcmp(pom_buffer, "=end\n")));

}

int is_keyword(char *string)
{
    for (int i = 0; i < 9; i++) {
        if (strcmp(string, keywords[i]) == 0) {
            return i + 5;
        }
    }

    return 0;

}


void create_token(char character, char *string, struct TToken *new_token, TType type) {

    TType keyword_type;
    ungetc(character, stdin);
    buffer[count - 1] = '\0';

    // todo
    if (type == T_VAR) {
        int i = 0;
        while (string[i] != '\0') {
            string[i] = (char)tolower(string[i]);
            i++;
        }
    }

    new_token->type = (keyword_type = (TType)is_keyword(string))
            ? keyword_type
            : type;


    if (new_token->type == T_INT) {
        char *ptr = malloc(sizeof(char));
        new_token->value.is_int = strtol(string, &ptr, 10);

    } else if (new_token->type  == T_FLOAT) {
        new_token->value.is_float = strtof(string, NULL);

    } else if (new_token->type  == T_VAR || new_token->type == T_FCE) {
        new_token->value.is_char = string;
        new_token->name = string;

    } else {
        new_token->name = NULL;
    }
}

int get_token() {

    static int lineNum = 0; /* number of line */
    static int linePos = 0; /* position */
    int allocated = 0;
    free(buffer);

    count = 0;
    buffer = NULL;
    TState state = START; /* initial state */


    while (1) {
        /* buffer realloc */
        buffer = realloc(buffer, (size_t) ++allocated);

        if (state != S_ERROR) {
            /* end of file at the beginning */
            if (((c = (char)getchar()) == EOF) && (state == START)) {
                token.type = T_IS_EOF;
                return TOKEN_OK;
            }

            linePos++;

            /* write to buffer without comments */
            if (!((state == START_BLOCK_COMMENT || state == END_BLOCK_COMMENT) && c != '=')) {
                buffer[count++] = c; /* save character to buffer */
            }
        } else {
            token.type = T_IS_ERR;
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
                    count = 0;
                    allocated = 0;
                    break;
                } /* new line*/
                else if (c == '\n') {
                    lineNum++;
                    linePos = 0;
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
                }
                else if (c == '+')
                {
                    state = S_ADD;
                    break;
                }
                else if (c == '-')
                {
                    state = S_SUB;
                    break;
                }
                else if (c == '*')
                {
                    state = S_MUL;
                    break;
                }
                else if (c == '/')
                {
                    state = S_DIV;
                    break;
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
                    state = S_IS_COMMA;
                    break;
                }
                else if (c == '(')
                {
                    state = S_LEFT_BRACKET;
                    break;
                }
                else if (c == ')')
                {
                    state = S_RIGHT_BRACKET;
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
                else
                    state = S_FLOAT;
                break;

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
                if (c == '!' || c == '?')
                {
                    state = S_FCE;
                    break;
                }
                else
                {
                    create_token(c, buffer, &token, T_VAR);
                    return 0;
                }
            case S_FCE:
                create_token(c, buffer, &token, T_FCE);
                return 0;

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

            case END_STRING:
                create_token(c, buffer, &token, T_STRING);
                return 0;

            /* mathematical operations */
            case S_ADD:
                create_token(c, buffer, &token, T_ADD);
                return 0;


            case S_SUB:
                create_token(c, buffer, &token, T_SUB);
                return 0;


            case S_MUL:
                create_token(c, buffer, &token, T_MUL);
                return 0;


            case S_DIV:
                create_token(c, buffer, &token, T_DIV);
                return 0;


            case S_SMALLER:
                if (c == '=')
                {
                    state = S_IS_SMALLER_OR_EQUAL;
                    break;
                }
                else
                {
                    create_token(c, buffer, &token, T_IS_SMALLER);
                    return 0;
                }


            case S_GREATER:
                if (c == '=')
                {
                    state = S_IS_GREATER_OR_EQUAL;
                    break;
                }
                else
                {
                    create_token(c, buffer, &token, T_IS_GREATER);
                    return 0;
                }


            case S_IS_SMALLER_OR_EQUAL:
                create_token(c, buffer, &token, T_IS_SMALLER_OR_EQUAL);
                return 0;


            case S_IS_GREATER_OR_EQUAL:
                create_token(c, buffer, &token, T_IS_GREATER_OR_EQUAL);
                return 0;


            case S_COMMENT_EQ_ASSIGNMENT:
                if (c == '=')
                {
                    state = S_EQUAL;
                    break;
                }
                else if (c == 'b' )
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
                    buffer[count++] = c;
                    if (is_comment_end()) {
                        linePos += 3;
                        if (c == '\n') {
                            lineNum++;
                            linePos = 0;
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
                        count = 0;
                        allocated = 0;
                        token.type = T_IS_EOL;
                    }
                    state = START_BLOCK_COMMENT;
                    break;
                }


            case END_BLOCK_COMMENT:
                if (c == '\n') {
                    lineNum++;
                    linePos = 0;
                    token.type = T_IS_EOL;
                    return 0;
                }
                break;

            case S_EQUAL:
                create_token(c, buffer, &token, T_IS_EQUAL);
                return 0;

            // todo ??
            case S_IS_NOT_EQUAL:
                if (c == '=') {
                    state = S_IS_NOT_EQUAL2;
                    break;
                } else
                    state = S_ERROR;
                break;

            case S_IS_NOT_EQUAL2:
                create_token(c, buffer, &token, T_IS_NOT_EQUAL);
                return 0;


            case S_IS_COMMA:
                create_token(c, buffer, &token, T_IS_COMMA);
                return 0;


            case S_ASSIGNMENT:
                create_token(c, buffer, &token, T_ASSIGNMENT);
                return 0;


            case S_LEFT_BRACKET:
                create_token(c, buffer, &token, T_LEFT_BRACKET);
                return 0;


            case S_RIGHT_BRACKET:
                create_token(c, buffer, &token, T_RIGHT_BRACKET);
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


            case S_ERROR:
                fprintf(stderr, "Spatny format na radku:%d (char:%d)!\n", lineNum, linePos);
                token.type = T_IS_ERR;
                return 1;


            default:
                state = S_ERROR;
                break;

        }
    }
}