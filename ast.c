/**
 * Formal Languages and Compilers 2019/2020
 * Implementation of the imperative language interpreter
 * @file ast.c
 * @brief source file for abstract syntactic tree
 * @author xhladi23 Martin Hladis
 * @author xprasi06 Matej Prasil
 * @author xvadur04 Martin Vadura
 * @author xsleza20 Alexandra Slezakova
 */

#include "ast.h"

void ast_init(Nnode *node)
{
    //*root = NULL;
    *node = NULL;

}



//void myast_destroy(Nnode node)
//{
//    if (node->nodes[node->data->size]==NULL && node->data->size != 0)
//    {
//        free(node->nodes[node->data->size]);
//        node->data->size--;
//        myast_destroy(node->nodes[node->data->size]);
//    }
//    else if (node->nodes[node->data->size]==NULL && node->data->size == 0)
//    {
//        free(node);
//        myast_destroy(node->parent_node);
//    }
//    else if (node->nodes[node->data->size]!=NULL)
//    {
//        myast_destroy(node->nodes[node->data->size]);
//    }
//}

void NstackFirstPush(NStack *s, Nnode node)
{
    for(int i = s->top - 1; i >= 0; i--)
    {
        s->nstack[i+1] = s->nstack[i];
    }
    s->nstack[0]=node;
    s->top += 1;
}

void ast_rename_value(char *com, Nnode ast, char *new_c)
{
    if(ast->data->data != NULL)
        if(!strcmp(com,ast->data->data))
            ast->data->data = new_c;

    for(int i = 0; ast->data->child_count > i; i++)
    {
        if (ast->children[i]!=NULL)
            ast_rename_value(com,ast->children[i],new_c);
    }
}

void NstackPopGround (NStack *s) {
    for (int i = 1; i <= s->top+1 && s->top != 0; i++) {
        s->nstack[i - 1] = s->nstack[i];
    }
    if(s->top >= 0)
        s->top--;
}

void NstackPop (NStack *s)
{
    if(s->top != -1)
    {
        s->nstack[s->top] = NULL;
        s->top -= 1;
    }
    //solved = 0;                      /* V případě řešení, smažte tento řádek! */
}
//předělat neřešit indent vracet přidanou větvu
void NstackPopAll (NStack *s)
{
    while(s->top != 0 && s->top > -1)
    {
        s->nstack[s->top] = NULL;
        s->top -= 1;
    }
    s->nstack[s->top] = NULL;
}


void NstackPush (NStack *s,Nnode node )
{

    if((s->top == 14) == 0 )
    {
        s->nstack[s->top] = node;
        s->top +=1;
    }
}

Nnode ast_add_node(Nnode *node, Ntype type, char *data ,bool inmain, int indent)
{
    Nnode new_node =  malloc(sizeof(struct Node));
    NData *new_data = malloc(sizeof(struct NDATA));
    new_node->data = new_data;
    new_node->data->data = data;
    new_node->data->ntype = type;
    new_node->data->indent = indent;
    new_node->data->size = TAB_SIZE;
    new_node->data->child_count=0;
    new_node->data->inmain = inmain;
    new_node->parent_node = *node;

    if ( *node == NULL && root == NULL)
    {
        *node = new_node;
        root =  new_node;
        return new_node;
    }
    else if ( *node == NULL && root != NULL )
    {
        return new_node;
    }
    (*node)->children[(*node)->data->child_count++] = new_node;
    return new_node;

}

Ntype node_type(S_ELEM *stack_elem)
{
    Nnode node = malloc(sizeof(struct Node));
    NData *data = malloc(sizeof(struct NDATA));
    node->data = data;
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