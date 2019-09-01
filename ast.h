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
    PSA_MULTIPLICATION,     /*  *  */
    PSA_DIVISION,           /*  /  */
    PSA_ADDITION,           /*  +  */
    PSA_SUBTRACTION,        /*  -  */
    PSA_LESS,               /*  <  */
    PSA_LESSEQUAL,          /*  <= */
    PSA_GREATER,            /*  >  */
    PSA_GREATEREQUAL,       /*  >= */
    PSA_EQUAL,              /*  == */
    PSA_NOTEQUAL,           /*  != */
    PSA_LBRACKET,           /*  (  */
    PSA_RBRACKET,           /*  )  */
    PSA_OPERAND,            /*  id */
    PSA_END,                /*  $  */

    NON_TERMINAL,           /*  E  */

    START_HANDLE,           /* '<' */
    END_HANDLE,             /* '>' */
    PSA_NONE,
    MATHEMATICAL_OPERATION
} PSA_SYMBOL;

typedef struct stackElem {
    struct TToken *current_token;
    struct stackElem *next;
    AST_NODE *node;
    int psa_symbol;
} S_ELEM;

AST_node_type node_type(S_ELEM *stack_elem);
