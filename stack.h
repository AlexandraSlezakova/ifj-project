#include "ast.h"

#ifndef IFJ_STACK_H
#define IFJ_STACK_H

typedef struct {
    S_ELEM *top;
} STACK;

void stack_init(STACK *stack);
void stack_pop (STACK *stack);
void stack_destroy(STACK *stack);
S_ELEM *stack_top(STACK *stack);
S_ELEM *top_terminal(STACK *stack);
int stack_push(STACK *stack, struct TToken *current_token, AST_NODE *node, int psa_symbol);
int stack_push_handle(STACK *stack, S_ELEM *elem, int psa_symbol);
int stack_top_rule(STACK *stack, PSA_SYMBOL *psa_buffer, S_ELEM *stack_elem);
bool stack_empty(STACK *stack);
bool is_terminal(S_ELEM *elem);

#endif //IFJ_STACK_H
