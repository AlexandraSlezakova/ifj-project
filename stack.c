/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file stack.c
 * @brief implementation of stack
 * @author
 * @author
 * @author
 * @author
 */
#include "stack.h"
#include <stdlib.h>

void stack_init(STACK *stack) {
    stack->top = NULL;
}

S_ELEM *stack_top(STACK *stack) {

    if (stack && stack->top) {
        S_ELEM *data = stack->top;
        stack->top = stack->top->next;
        return data;
    }

    return NULL;
}

S_ELEM *top_terminal(STACK *stack) {
    S_ELEM *terminal;
    if (stack && stack->top) {
        terminal = stack->top;
        while (terminal) {
            if (is_terminal(terminal)) {
                return terminal;
            }
            terminal = terminal->next;
        }
    }

    return NULL;
}

bool is_terminal(S_ELEM *elem) {
    return elem->psa_symbol <= PSA_END;
}

int stack_push(STACK *stack, struct TToken *current_token, Nnode *node, PSA_SYMBOL psa_symbol, DATA_TYPE type) {
    IF_RETURN(!stack, ERR_INTERNAL)

    S_ELEM *stack_elem = malloc(sizeof(S_ELEM));
    IF_RETURN(!stack_elem, ERR_INTERNAL)

    if (current_token != NULL) {
        stack_elem->current_token = current_token;
    }

    stack_elem->next = stack->top;
    stack_elem->node = node;
    stack_elem->psa_symbol = psa_symbol;
    stack_elem->type = type;

    stack->top = stack_elem;

    return OK;
}

bool stack_empty(STACK *stack) {
    return stack->top == NULL;
}

int stack_push_handle(STACK *stack, S_ELEM *elem, PSA_SYMBOL psa_symbol) {
    IF_RETURN(!stack, ERR_INTERNAL)

    S_ELEM *new_elem = malloc(sizeof(S_ELEM));
    IF_RETURN(!new_elem, ERR_INTERNAL)

    if (stack->top && stack->top->psa_symbol == NON_TERMINAL) {
        new_elem = stack->top;

        S_ELEM *copy = malloc(sizeof(S_ELEM));
        copy->psa_symbol = elem->psa_symbol;
        copy->next = elem->next;
        copy->type = elem->type;

        new_elem->next->psa_symbol = psa_symbol;
        new_elem->next->next = copy;
        //new_elem->next->next->psa_symbol = copy->psa_symbol;
        //new_elem->next->next->next = copy->next;
    } else {
        new_elem->psa_symbol = psa_symbol;
        new_elem->next = elem;
    }

    stack->top = new_elem;

    return OK;
}

int stack_push_indent(STACK *stack, int count_indent, TType indent_type) {

    IF_RETURN(!stack, ERR_INTERNAL)

    S_ELEM *new_elem = malloc(sizeof(S_ELEM));
    IF_RETURN(!new_elem, ERR_INTERNAL)

    new_elem->next = stack->top;
    new_elem->indent_counter = count_indent;
    new_elem->indent_type = indent_type;

    stack->top = new_elem;

    return OK;

}

int stack_top_rule(STACK *stack, PSA_SYMBOL psa_buffer[4], S_ELEM stack_elem[4], struct TToken *previous) {

    S_ELEM *tmp = NULL;
    int i = 0;

    if (stack && stack->top) {
        tmp = stack->top;
        while (tmp && tmp->psa_symbol != START_HANDLE) {
            IF_RETURN((i == 4 || i == PSA_END), 1)

            if (tmp->psa_symbol >= PSA_MULTIPLICATION && tmp->psa_symbol <= PSA_NOTEQUAL) {
                psa_buffer[i] = MATHEMATICAL_OPERATION;
            } else {
                psa_buffer[i] = tmp->psa_symbol;
            }
            stack_elem[i].psa_symbol = tmp->psa_symbol;
            stack_elem[i].current_token = previous;
            stack_elem[i].type = tmp->type;

            stack_elem[i].next = tmp->next;
            tmp = tmp->next;
            i++;
        }

        IF_RETURN(!tmp, 1)
    }

    return OK;
}

void stack_pop(STACK *stack)
{
    if (stack && stack->top) {
        S_ELEM *elem = stack->top;
        stack->top = stack->top->next;
        free(elem);
    }
}

void stack_destroy(STACK *stack)
{
    S_ELEM *elem;

    if (stack && stack->top) {
        while (stack->top) {
            elem = stack->top;
            stack->top = stack->top->next;
            free(elem);
        }
    }
}
