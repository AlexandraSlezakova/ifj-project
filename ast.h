/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file myast.h
 * @brief header file of myast.c
 * @author xhladi23 Martin Hladis
 * @author xprasi06 Matej Prasil
 * @author xvadur04 Martin Vadura
 * @author xsleza20 Alexandra Slezakova
 */

#include <stdbool.h>
#include "functions.h"
#include "errors.h"

#ifndef SCANNER_C_MYAST_H
#define SCANNER_C_MYAST_H

#endif //SCANNER_C_MYAST_H
#define TAB_SIZE 101

typedef enum Node_Type{
    PROG        = 1,
    VAR         = 2,        //Var
    VAL         = 3,        //Value
    IF_NODE     = 4,
    IF_BODY     = 5,
    IF_ELSE     = 6,
    WHILE       = 7,
    BODY        = 8,
    CALL        = 9,
    COND        = 10,       // Condition
    RETURN      = 11,       // Return
    ADD         = 12,       // +
    SUB         = 13,       // -
    MUL         = 14,       // *
    DIV         = 15,       // /
    DIVINIT     = 16,       // //
    GR          = 17,       // <>
    GEQ         = 18,       // <= >=
    LESS        = 19,       // ><
    LOQ         = 20,       // >= <=
    COMP        = 21,       // ==
    NOTCOMP     = 22,       // !=
    ASSIGN      = 23,       // =
    FUNC_DEF    = 24,
    ARGV        = 25,
    VAR_DEF     = 26,
    NO_NODE     = 27,
    PARAM       = 28,
} Ntype;

typedef struct NDATA{
    int indent;
    Ntype ntype;
    char  *data;
    bool inmain;
    int size;
    int child_count;
}NData;

typedef struct Node{
    NData *data;
    struct Node *children[TAB_SIZE];
    struct Node *parent_node;
} *Nnode;

typedef struct {
    struct Node *nstack[TAB_SIZE];
    int top;
} NStack;

NStack* nStack;
NStack* Arr_Nstack;

void NstackFirstPush(NStack *s, Nnode node);
void NstackPopGround (NStack *s);
void NstackPopAll (NStack *s);
void NstackPop (NStack *s);
void NstackPush (NStack *s,Nnode node);
void ast_init(Nnode *node);
Nnode ast_add_node(Nnode *node, Ntype type, char *data, bool inmain ,int indent) ;
void ast_rename_value(char *com, Nnode ast, char *new_c);

Nnode root;

typedef enum {
    PSA_MULTIPLICATION      = 0,     /*  *  */
    PSA_DIVISION            = 1,     /*  /  */
    PSA_DIVISION_INT        = 2,     /*  // */
    PSA_ADDITION            = 3,     /*  +  */
    PSA_SUBTRACTION         = 4,     /*  -  */
    PSA_LESS                = 5,     /*  <  */
    PSA_LESSEQUAL           = 6,     /*  <= */
    PSA_GREATER             = 7,     /*  >  */
    PSA_GREATEREQUAL        = 8,     /*  >= */
    PSA_EQUAL               = 9,     /*  == */
    PSA_NOTEQUAL            = 10,    /*  != */
    PSA_LBRACKET            = 11,    /*  (  */
    PSA_RBRACKET            = 12,    /*  )  */
    PSA_OPERAND             = 13,    /*  id */
    PSA_END                 = 14,    /*  $  */

    NON_TERMINAL            = 16,    /*  E  */

    START_HANDLE            = 18,    /* '<' */
    END_HANDLE              = 19,    /* '>' */
    PSA_NONE                = 20,
    MATHEMATICAL_OPERATION  = 21
} PSA_SYMBOL;

typedef enum
{
    TYPE_STRING,
    TYPE_FLOAT,
    TYPE_INTEGER,
    TYPE_NIL,
    TYPE_IDENTIFIER,
    TYPE_UNKNOWN,
    UNDEFINED
} DATA_TYPE;


typedef struct stackElem {
    struct TToken *current_token;
    struct stackElem *next;
    Nnode *node;
    PSA_SYMBOL psa_symbol;
    DATA_TYPE type;
    TType indent_type;      /* indent or dedent */
    int indent_counter;     /* number of spaces - indentation level */
} S_ELEM;

Ntype node_type(S_ELEM *stack_elem);
