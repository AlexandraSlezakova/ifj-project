//
// Created by god on 13.10.19.
//


#include <stdbool.h>
#include "functions.h"
#include "errors.h"

#ifndef SCANNER_C_MYAST_H
#define SCANNER_C_MYAST_H

#endif //SCANNER_C_MYAST_H


typedef enum Node_Type{
    Prog,
    Var,            //Var
    Val,            //Value
    If_Node,
    If_Body,
    If_Else,
    While,
    Body,
    Call,
    Cond,           // Condition
    Ret,            // Return
    Add,            // +
    Sub,            // -
    Mul,            // *
    Div,            // /
    DivInit,        // //
    Greater,        // <>
    GreOrEq,        // <= >=
    Less,           // ><
    LessOrEq,        // >= <=
    Comp,            // ==
    NotComp,        // !=
    Assign,          // =
    Func_Def,
    Argv,
    Var_Def,
    Condition,
    No_Node,
    Return,
} Ntype;

typedef struct NDATA{
    int indent;
    Ntype ntype;
    char  *data;
    bool inmain;
    int size;
}*NData;

typedef struct Node{
    NData data;
    struct Node **nodes;
    struct Node *parent_node;
}*Nnode;

Nnode myast_init(Nnode node);
void myast_destroy(Nnode node);
Nnode* myast_add_node(Nnode node, Ntype type, char *data, int indent, bool inmain);


Nnode *root;

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