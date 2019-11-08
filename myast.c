/**
 * Formal Languages and Compilers 2019/2020
 * Implementation of the imperative language interpreter
 * @file ast.c
 * @brief source file for abstract syntactic tree
 * @author
 * @author
 * @author
 * @author
 */

#include "myast.h"

Nnode myast_init(Nnode *node)
{
    root = NULL;
    node = NULL;
    return node;
}



void myast_destroy(Nnode node)
{
    if (node->nodes[node->data->size]==NULL && node->data->size != 0)
    {
        free(node->nodes[node->data->size]);
        node->data->size--;
        myast_destroy(node->nodes[node->data->size]);
    }
    else if (node->nodes[node->data->size]==NULL && node->data->size == 0)
    {
        free(node);
        myast_destroy(node->parent_node);
    }
    else if (node->nodes[node->data->size]!=NULL)
    {
        myast_destroy(node->nodes[node->data->size]);
    }
}

/*
Nnode* find_node (Nnode node, int indent)
{
    while(node[node->data->size].data->indent != indent )
        node = node->parent_node;

    return node;

}*/

//předělat neřešit indent vracet přidanou větvu

Nnode* myast_add_node(Nnode *node, Ntype type, char *data,bool inmain, int indent)
{
    Nnode new_node = malloc(sizeof(struct Node *));
    NData new_data = malloc(sizeof(struct Ndata *));
    new_node->data=new_data;
    new_node->data->data=data;
    new_node->data->ntype = type;
    new_node->data->indent = indent;
    new_node->data->size=0;
    new_data->inmain = inmain;

    if (node == NULL)
    {
        node = (Nnode)malloc(sizeof(struct Node *));
        (*node)->data = malloc(sizeof(struct Ndata*));
        (*node)->nodes = malloc(sizeof(struct Node *)*10);
        //node->nodes[0] = new_node;
        (*node)->data->size = 0;
        root = node;
        return node;
    }
    (*node)->nodes[(*node)->data->size] = new_node;
     return (*node)->nodes[(*node)->data->size];
}

Ntype node_type(S_ELEM *stack_elem)
{
    Nnode node;
    switch (stack_elem->psa_symbol) {
        case PSA_MULTIPLICATION:
            node->data->ntype = MUL;
            break;
        case PSA_DIVISION:
            node->data->ntype = DIV;
            break;
        case PSA_DIVISION_INT:
            node->data->ntype= DIVINIT;
            break;
        case PSA_ADDITION:
            node->data->ntype= ADD;
            break;
        case PSA_SUBTRACTION:
            node->data->ntype = SUB;
            break;
        case PSA_LESS:
            node->data->ntype= LESS;
            break;
        case PSA_LESSEQUAL:
            node->data->ntype = LOQ;
            break;
        case PSA_GREATER:
            node->data->ntype = GR;
            break;
        case PSA_GREATEREQUAL:
            node->data->ntype = GEQ;
            break;
        case PSA_EQUAL:
            node->data->ntype = COMP;
            break;
        case PSA_NOTEQUAL:
            node->data->ntype = NOTCOMP;
            break;
        default:
            node->data->ntype = NO_NODE;
            break;
    }

    return node->data->ntype;
}