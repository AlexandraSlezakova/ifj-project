/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file stack.h
 * @brief header file of stack.c
 * @author
 * @author
 * @author
 * @author
 */
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
int stack_push(STACK *stack, struct TToken *current_token, AST_NODE *node, PSA_SYMBOL psa_symbol, DATA_TYPE type);
int stack_push_handle(STACK *stack, S_ELEM *elem, PSA_SYMBOL psa_symbol);
int stack_top_rule(STACK *stack, PSA_SYMBOL psa_buffer[4], S_ELEM stack_elem[4], struct TToken *previous);
bool stack_empty(STACK *stack);
bool is_terminal(S_ELEM *elem);

#endif //IFJ_STACK_H