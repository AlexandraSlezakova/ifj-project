/**
 * Formal Languages and Compilers 2018/2019
 * Implementation of the imperative language interpreter
 * @file ast.h
 * @brief header file for abstract syntactic tree
 * @author Soroka Matej <xsorok02>
 * @author Hanák Karel <xhanak34>
 * @author Sedlák David <xsedla1d>
 * @author Procházka Patrik <xproch97>
 */

#include <stdbool.h>
#include "functions.h"
#include "errors.h"

typedef enum {
    PROG,       /**< PROG     */
    ADD,        /**< +        */
    SUB,        /**< -        */
    MUL,        /**< *        */
    DIV,        /**< /        */
    DIV_INT,    /**< //       */
    GREATER,    /**< >        */
    GEQ,        /**< >=       */
    LESS,       /**< <        */
    LEQ,        /**< <=       */
    COMP,       /**< ==       */
    NEQ,        /**< !=       */
    VAR,        /**< variable */
    VAL,        /**< value    */
    IF_NODE,    /**< if       */
    IF_BODY,
    ELSE_BODY,
    WHILE_NODE, /**< while    */
    BODY,       /**< body     */
    CALL,       /**< call     */
    FUNC_DEF,   /**< function definition */
    ARG,        /**< argument of function call */
    VARDEF,     /**< DEFVAR */
    ASSIGN,     /**< = */
    BODY_TO_PROCESS, /**< BODY PROCESSED WITH BUFFER */
    CONDITION,
    NO_NODE     =   100
} AST_node_type;


typedef struct AST_node {
    AST_node_type operation;
    struct AST_node **childs;
    char *data;
    int child_count;
    int size;
    bool in_main;
    bool is_builtin;
} AST_NODE;

int
ast_resize(AST_NODE *tree_node);

bool
ast_is_full(AST_NODE *tree_node);

AST_NODE *
ast_add_node(AST_NODE **tree_node, AST_node_type operation, char *data, bool in_main);

void
ast_init(AST_NODE **tree_node);

void
ast_destroy(AST_NODE **tree);

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
    AST_NODE *node;
    PSA_SYMBOL psa_symbol;
    DATA_TYPE type;
    TType indent_type;      /* indent or dedent */
    int indent_counter;     /* number of spaces - indentation level */
} S_ELEM;

AST_node_type node_type(S_ELEM *stack_elem);
