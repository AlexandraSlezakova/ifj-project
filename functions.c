/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file functions.c
 * @brief functions
 * @author
 * @author
 * @author
 * @author
 */

#include "functions.h"

bool is_eol(TType type)
{
    return type == T_IS_EOL;
}

bool is_identifier(TType type)
{
    return type == T_VAR || type == T_FCE;
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
        || type == T_FCE
        || type == T_NIL
        || type == T_LEFT_BRACKET;
}

bool is_comma(TType type)
{
    return type == T_IS_COMMA;
}

bool is_function_call(struct TToken current_token, char *name)
{
    if (current_token.type == T_VAR || current_token.type == T_FCE) {
        *name = *current_token.name;
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
           || type == T_VAR
           || type == T_NIL;
}

bool is_global_scope(int scope)
{
    return scope == GLOBAL_SCOPE;
}