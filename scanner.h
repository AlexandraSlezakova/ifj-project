/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file scanner.h
 * @brief header file for scanner
 * @author
 * @author
 * @author
 * @author
 */
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <memory.h>
#include <stdbool.h>

#ifndef SCANNER_H
#define SCANNER_H

typedef enum {
    START,                      /* pociatocny stav */
    S_INT,
    S_DEC_SEP,                  /* desatinna bodka */
    S_EXP,                      /* exponent */
    S_FLOAT,                    /* cislo s desatinnou bodkou */
    S_EXP_CHAR,                 /* nepovinne znameniko +/- za exponentom */
    S_NUMBER_END,               /* koniec ciselneho literalu */
    S_VAR,                      /* identifikator */
    S_FCE,                      /* identifikator funkcie */
    START_STRING,               /* zaciatok retazca */
    S_ESC,                      /* escape sekvencia v retazci */
    S_STRING,                   /* retazec */
    END_STRING,                 /* koniec retazca */
    S_COMMENT,                  /* riadkovy komentar */
    S_COMMENT_EQ_ASSIGNMENT,    /* znak rovnosti na zaciatok blokoveho komentara */
    S_EQUAL,                    /* znak rovnosti */
    START_BLOCK_COMMENT,        /* blokovy komentar */
    END_BLOCK_COMMENT,          /* koniec blokoveho komentara */
    S_ASSIGNMENT,
    S_ADD,
    S_SUB,
    S_ZERO,
    S_MUL,
    S_DIV,
    S_SMALLER,
    S_GREATER,
    S_IS_SMALLER_OR_EQUAL,
    S_IS_GREATER_OR_EQUAL,
    S_IS_NOT_EQUAL,
    S_IS_NOT_EQUAL2,
    S_IS_COMMA,
    S_LEFT_BRACKET,
    S_RIGHT_BRACKET,
    S_HEX_ESCAPE,
    S_HEX_ESCAPE2,
    S_ERROR
}TState;

typedef enum {
    T_VAR,			        /* 0  */
    T_FCE,	                /* 1  */
    T_INT,		            /* 2  */
    T_FLOAT,	            /* 3  */
    T_STRING,	            /* 4  */

    /* keywords */
    T_DEF,			        /* 5  */
    T_DO,			        /* 6  */
    T_WHILE,                /* 7  */
    T_IF,                   /* 8  */
    T_THEN,                 /* 9  */
    T_ELSE,                 /* 10 */
    T_NOT,			        /* 11 */
    T_END,                  /* 12 */
    T_NIL,                  /* 13 */

    /* mathematical operations */
    T_ADD,			        /* 14 */
    T_SUB,			        /* 15 */
    T_MUL,			        /* 16 */
    T_DIV,			        /* 17 */
    T_IS_SMALLER,		    /* 18 */
    T_IS_GREATER,		    /* 19 */
    T_IS_SMALLER_OR_EQUAL,	/* 20 */
    T_IS_GREATER_OR_EQUAL,	/* 21 */
    T_IS_EQUAL,			    /* 22 */
    T_IS_NOT_EQUAL,		    /* 23 */

    /* others */
    T_IS_COMMA,			    /* 24 */
    T_ASSIGNMENT,           /* 25 */
    T_LEFT_BRACKET,         /* 26 */
    T_RIGHT_BRACKET,        /* 27 */
    T_IS_EOF,			    /* 28 */
    T_IS_EOL,			    /* 29 */
    T_IS_ERR			    /* 30 */
}TType;

struct TToken {
    TType type;
    union {
        int is_int;
        float is_float;
        char *is_char;
    }value;
    char *name;
} token;

char c;
int get_token();
void create_token(char character, char *string,  struct TToken *new_token, TType type);
int is_keyword(char *string);
char *buffer;
int count;
#endif