/**
 * Formal Languages and Compilers 2018/2019
 * Implementation of the imperative language interpreter
 * @file ast.c
 * @brief source file for abstract syntactic tree
 * @author Soroka Matej <xsorok02>
 * @author Hanák Karel <xhanak34>
 * @author Sedlák David <xsedla1d>
 * @author Procházka Patrik <xproch97>
 */

#include "ast.h"
#include <stdbool.h>
#include <stdlib.h>

int
ast_resize(AST_NODE *tree_node)
{
    AST_NODE **tmp = NULL;

    tmp = realloc(tree_node->childs, 2 * sizeof(AST_NODE*) * tree_node->size);
    IF_RETURN(!tmp, ERR_INTERNAL)

    tree_node->childs = tmp;
    tree_node->size *= 2;

    /* initialize memmory after reallocation */
    memset(&(tree_node->childs[tree_node->child_count]), 0, sizeof(AST_NODE *) * (tree_node->size - tree_node->child_count));

    return OK;
}

bool
ast_is_full(AST_NODE *tree_node)
{
    return tree_node->size == tree_node->child_count;
}

AST_NODE *
ast_add_node(AST_NODE **tree_node, AST_node_type operation, char *data, bool in_main)
{
    AST_NODE *ret = NULL;

    if (!(*tree_node)) {
        *tree_node = calloc(1, sizeof(**tree_node));
        IF_RETURN(!(*tree_node), NULL);
        (*tree_node)->childs = calloc(2, sizeof(AST_NODE *));
        if (!(*tree_node)->childs) {
            free(*tree_node);
            *tree_node = NULL;
            return NULL;
        }
        (*tree_node)->child_count = 0;
        (*tree_node)->operation = operation;
        (*tree_node)->size = 2;
        (*tree_node)->data = data;
        (*tree_node)->in_main = in_main;
        ret = (*tree_node);
    } else {
        if (ast_is_full(*tree_node)) {
            IF_RETURN(ast_resize(*tree_node) != OK, NULL);
        }
        ret = ast_add_node(&((*tree_node)->childs[(*tree_node)->child_count]), operation, data, in_main);
        (*tree_node)->child_count++;
    }

    return ret;
}

AST_NODE *
ast_add_node_at_start(AST_NODE **tree_node, AST_node_type operation, char *data, bool in_main)
{
    AST_NODE *ret = NULL;
    AST_NODE *new_first = NULL;
    ret = ast_add_node(tree_node, operation, data, in_main);
    IF_RETURN(!ret, NULL);

    new_first = (*tree_node)->childs[(*tree_node)->child_count - 1];
    memmove(&((*tree_node)->childs[1]), &((*tree_node)->childs[0]), sizeof(AST_NODE*) * ((*tree_node)->child_count - 1));
    (*tree_node)->childs[0] = new_first;
    return new_first;
}

void
ast_init(AST_NODE **tree_node)
{
    (*tree_node) = NULL;
}

void
ast_destroy(AST_NODE **tree)
{
    if (!tree || !(*tree)) {
        return;
    }

    for(int i = 0; i < (*tree)->child_count; i++) {
        ast_destroy(&((*tree)->childs[i]));
    }
    free((*tree)->childs);
    free((*tree)->data);
    free(*tree);
    (*tree) = NULL;
}

AST_node_type node_type(S_ELEM *stack_elem)
{
    AST_node_type node;
    switch (stack_elem->psa_symbol) {
        case PSA_MULTIPLICATION:
            node = MUL;
            break;
        case PSA_DIVISION:
            node = DIV;
            break;
        case PSA_DIVISION_INT:
            node = DIV_INT;
            break;
        case PSA_ADDITION:
            node = ADD;
            break;
        case PSA_SUBTRACTION:
            node = SUB;
            break;
        case PSA_LESS:
            node = LESS;
            break;
        case PSA_LESSEQUAL:
            node = LEQ;
            break;
        case PSA_GREATER:
            node = GREATER;
            break;
        case PSA_GREATEREQUAL:
            node = GEQ;
            break;
        case PSA_EQUAL:
            node = COMP;
            break;
        case PSA_NOTEQUAL:
            node = NEQ;
            break;
        default:
            node = NO_NODE;
            break;
    }

    return node;
}