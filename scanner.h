/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file scanner.h
 * @brief header file for scanner
 * @author xhladi23 Martin Hladis
 * @author xprasi06 Matej Prasil
 * @author xvadur04 Martin Vadura
 * @author xsleza20 Alexandra Slezakova
 */
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <memory.h>
#include <stdbool.h>

#include "errors.h"

#ifndef IFJ_SCANNER_H
#define IFJ_SCANNER_H

typedef enum {
    START,                      /* pociatocny stav */
    S_INT,
    S_DEC_SEP,                  /* desatinna bodka */
    S_EXP,                      /* exponent */
    S_EXP_CHAR,                 /* nepovinne znamienko +/- za exponentom */
    S_FLOAT,                    /* cislo s desatinnou bodkou */
    S_NUMBER,                   /* ciselny literalu */
    S_VAR,                      /* identifikator */
    START_STRING,               /* zaciatok retazca */
    S_ESC,                      /* escape sekvencia v retazci */
    S_STRING_CONTENT,           /* obsah retazca */
    S_STRING,                   /* retazec */
    S_LINE_COMMENT,             /* riadkovy komentar */
    S_ASSIGN,                    /* znak rovnosti */
    S_IS_EQUAL,                 /*       ==      */
    S_START_DOC_1,              /* druha uvodzovka pre dokumentacny retazec */
    S_START_DOC_2,              /* tretia uvodzovka pre dokumentacny retazec */
    S_DOC_CONTENT,              /* obsah komentara */
    S_END_DOC_1,                /* koniec - druha uvodzovka pre dokumentacny retazec */
    S_END_DOC_2,                /* koniec - tretia uvodzovka pre dokumentacny retazec */
    S_COLON,                    /* dvojbodka */
    S_TAB,                      /* tabulator */
    S_SPACE,                    /* medzera */
    S_MATH_OP,                  /* matematicka operacia */
    S_DIV_INT,                  /* // */
    S_ZERO,
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
    S_ERROR,
    S_INDENT_ERROR,
    S_IS_EOL,
    S_DOC_STRING,
} TState;

typedef enum {
    T_VAR,			        /* 0  */
    T_INT,		            /* 1  */
    T_FLOAT,	            /* 2  */
    T_STRING,	            /* 3  */

    /* keywords */
    T_DEF,			        /* 4  */
    T_ELSE,			        /* 5  */
    T_IF,                   /* 6  */
    T_NONE,                 /* 7  */
    T_PASS,                 /* 8 */
    T_RETURN,               /* 9 */
    T_WHILE,    	        /* 10 */

    /* mathematical operations */
    T_ADD,			        /* 11 */
    T_SUB,			        /* 12 */
    T_MUL,			        /* 13 */
    T_DIV,			        /* 14 */
    T_IS_SMALLER,		    /* 16 */
    T_IS_GREATER,		    /* 17 */
    T_IS_SMALLER_OR_EQUAL,	/* 18 */
    T_IS_GREATER_OR_EQUAL,	/* 19 */
    T_IS_EQUAL,			    /* 20 */
    T_IS_NOT_EQUAL,		    /* 21 */

    /* others */
    T_IS_COMMA,			    /* 22 */
    T_ASSIGNMENT,           /* 23 */
    T_LEFT_BRACKET,         /* 24 */
    T_RIGHT_BRACKET,        /* 25 */
    T_IS_EOF,			    /* 26 */
    T_IS_EOL,			    /* 27 */
    T_IS_ERR,			    /* 28 */
    T_IS_COLON,             /* 29 */
    T_INDENT,               /* 30 */
    T_DEDENT,               /* 31 */
    T_UNKNOWN,
    T_DIV_INT,
} TType;

struct TToken {
    TType type;
    union {
        int is_int;
        float is_float;
        char *is_char;
    }value;
} token;

int get_token();
int get_length(int character);
int is_keyword(char *string);
void unget_token();
void create_token(int character, char *string,  struct TToken *new_token, TType type);
char *buffer;
int c;
int eol_flag;
int iterator;
int indent_counter;
int line_counter;        /* number of lines */
int line_position;       /* position */
TState previous_state;

#endif
