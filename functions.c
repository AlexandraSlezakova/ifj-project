/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file functions.c
 * @brief functions
 * @author xhladi23 Martin Hladis
 * @author xprasi06 Matej Prasil
 * @author xvadur04 Martin Vadura
 * @author xsleza20 Alexandra Slezakova
 */

#include "functions.h"

bool is_eol(TType type)
{
    return type == T_IS_EOL;
}

bool is_identifier(TType type)
{
    return type == T_VAR;
}

bool is_left_bracket(TType type)
{
    return type == T_LEFT_BRACKET;
}

bool is_right_bracket(TType type)
{
    return type == T_RIGHT_BRACKET;
}

bool is_assignment_correct(TType type)
{
    return type == T_STRING
        || type == T_FLOAT
        || type == T_INT
        || type == T_VAR
        || type == T_NONE
        || type == T_LEFT_BRACKET;
}

bool is_comma(TType type)
{
    return type == T_IS_COMMA;
}

bool is_function_call(struct TToken current_token, char *name)
{
    if (current_token.type == T_VAR) {
        *name = *current_token.value.is_char;
        get_token();
        if (is_left_bracket(token.type)) return true;
    }

    return false;
}

bool is_term(TType type)
{
    return type == T_STRING
           || type == T_FLOAT
           || type == T_INT
           || type == T_NONE
           || type == T_VAR;
}

bool is_global_scope(int scope)
{
    return scope == GLOBAL_SCOPE;
}

bool is_operator(TType type)
{
    return type == T_IS_SMALLER
        || type == T_IS_GREATER
        || type == T_IS_SMALLER_OR_EQUAL
        || type == T_IS_GREATER_OR_EQUAL
        || type == T_IS_NOT_EQUAL
        || type == T_IS_EQUAL
        || type == T_MUL
        || type == T_DIV
        || type == T_DIV_INT
        || type == T_ADD
        || type == T_SUB;
}