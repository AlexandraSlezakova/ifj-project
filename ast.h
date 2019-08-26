#ifndef IFJ_AST_H
#define IFJ_AST_H
#include "functions.h"
#include "errors.h"

typedef enum {
    IF_NODE,
    ELSEIF_NODE,
    ELSE_NODE,
    WHILE_NODE,
    DO_NODE,
    RETURN_NODE,
    FOR_NODE,
    FUNCTION_CALL
} AST_NODE_TYPE;

typedef struct ast_node {
    int count_child;
    AST_NODE_TYPE type;
    struct ast_node *condition;
    struct ast_node *branch;
    int elseif_count;
    int count_params;
    char *ret_type;
} AST_NODE;
#endif //IFJ_AST_H
