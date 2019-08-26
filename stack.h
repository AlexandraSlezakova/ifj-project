//
// Created by alexandra on 26.8.2019.
//

#include "ast.h"

#ifndef IFJ_STACK_H
#define IFJ_STACK_H

typedef struct stackElem {
    int data;
    struct stackElem *next;
    AST_NODE *node;
} S_ELEM;

typedef struct {
    S_ELEM *top;
} STACK;

void init_stack(STACK *stack);

S_ELEM *stack_pop(STACK *stack);

int stack_push(STACK *stack, int data, AST_NODE *node);

#endif //IFJ_STACK_H
