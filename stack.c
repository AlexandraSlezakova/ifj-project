#include "stack.h"
#include <stdlib.h>

void init_stack(STACK *stack) {
    stack->top = NULL;
}

S_ELEM *stack_pop(STACK *stack) {

    if (stack) {
        S_ELEM *data = stack->top;
        stack->top = stack->top->next;
        return data;
    }

    return NULL;
}

int stack_push(STACK *stack, int data, AST_NODE *node) {
    IF_RETURN(!stack, ERR_INTERNAL)

//    S_ELEM *stack_elem = malloc(sizeof(S_ELEM));
//    IF_RETURN(!stack_elem, ERR_INTERNAL)
//    stack_elem->data = data;
//    stack_elem->next = stack->top;
//    stack_elem->node = node;
//    stack->top = stack_elem;

    return OK;
}


