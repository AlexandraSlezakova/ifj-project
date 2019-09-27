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
    GREATER,    /**< >        */
    GEQ,        /**< >=       */
    LESS,       /**< <        */
    LEQ,        /**< <=       */
    COMP,       /**< ==       */
    NEQ,        /**< !=       */
    VAR,        /**< variable */
    VAL,        /**< value    */
    IF_NODE,    /**< if       */
    WHILE_NODE, /**< while    */
    BODY,       /**< body     */
    CALL,       /**< call     */
    FUNC_DEF,   /**< function definition */
    ARG,        /**< argument of function call */
    VARDEF,     /**< DEFVAR */
    ASSIGN,     /**< = */
    BODY_TO_PROCESS, /**< BODY PROCESSED WITH BUFFER */
    /* TODO define more operations */
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
    PSA_ADDITION            = 2,     /*  +  */
    PSA_SUBTRACTION         = 3,     /*  -  */
    PSA_LESS                = 4,     /*  <  */
    PSA_LESSEQUAL           = 5,     /*  <= */
    PSA_GREATER             = 6,     /*  >  */
    PSA_GREATEREQUAL        = 7,     /*  >= */
    PSA_EQUAL               = 8,     /*  == */
    PSA_NOTEQUAL            = 9,     /*  != */
    PSA_LBRACKET            = 10,    /*  (  */
    PSA_RBRACKET            = 11,    /*  )  */
    PSA_OPERAND             = 12,    /*  id */
    PSA_END                 = 13,    /*  $  */

    NON_TERMINAL            = 15,    /*  E  */

    START_HANDLE            = 16,    /* '<' */
    END_HANDLE              = 17,    /* '>' */
    PSA_NONE                = 18,
    MATHEMATICAL_OPERATION  = 19
} PSA_SYMBOL;

typedef enum
{
    TYPE_STRING,
    TYPE_FLOAT,
    TYPE_INTEGER,
    TYPE_NIL,
    TYPE_IDENTIFIER,
    TYPE_UNKNOWN
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
