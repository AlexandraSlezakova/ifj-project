/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file functions.h
 * @brief header file for functions
 * @author
 * @author
 * @author
 * @author
 */

#include "scanner.h"

#ifndef IFJ_FUNCTIONS_H
#define IFJ_FUNCTIONS_H

#define IF_RETURN(CONDITION, VAL) if (CONDITION) return VAL;
#define GLOBAL_SCOPE 0
#define SCOPE 1

bool is_eol(TType type);
bool is_identifier(TType type);
bool is_left_bracket(TType type);
bool is_right_bracket(TType type);
bool is_assignment_correct(TType type);
bool is_comma(TType type);
bool is_function_call(struct TToken current_token, char *name);
bool is_term(TType type);
bool is_global_scope(int scope);
bool is_operator(TType type);
bool is_tab (TType type);
char *copy_name(char *name);

#endif //IFJ_FUNCTIONS_H
