#include "functions.h"

/**
 * Function returns copy of name
 * @param name
 * @return
 */
char *copy_name(char *name) {
    char *copy = malloc(sizeof(char));
    strcpy(copy, name);

    return copy;
}

bool is_eol(TType type) {
    return type == TEOL;
}

bool is_identifier(TType type) {
    return type == TID || type == TIDFnc;
}

bool is_left_bracket(TType type) {
    return type == Tleft_bracket;
}

bool is_right_bracket(TType type) {
    return type == Tright_bracket;
}

bool is_assignment_correct(TType type) {
    return type == TFLOAT_VALUE
        || type == TSTRING_VALUE
        || type == TINT_VALUE
        || type == TID
        || type == TIDFnc;
}

bool is_comma(TType type) {
    return type == Tcarka;
}