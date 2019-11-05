/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file parser.c
 * @author
 * @author
 * @author
 * @author
 */
#include "parser.h"
#include "scanner.h"

int function_arguments(HTable *function_symtable, char *function_name)
{

    tDLList *arg_list = NULL;
    HTItem *function = ht_search(global_hashtable, function_name);
    IF_RETURN(!function, ERR_INTERNAL)

    arg_list = DLInitList();

    int countParams = 0;

    /* function has arguments */
    if (token.type == T_VAR) {

        char *name = malloc(sizeof(char));
        strcpy(name, token.value.is_char);
        /* insert variable name into function table */
        int inserted = insert_variable(function_symtable, name, TYPE_UNKNOWN);
        IF_VALUE_RETURN(inserted)

        /* insert argument name to list of arguments */
        IF_RETURN(!DLInsertLast(arg_list, name), ERR_INTERNAL)

        /* quantity of params */
        countParams++;

        IF_RETURN(get_token(), TOKEN_ERR)

        while (token.type != T_RIGHT_BRACKET) {
            IF_RETURN(!is_comma(token.type), SYNTAX_ERR)
            IF_RETURN(get_token(), TOKEN_ERR)

            IF_RETURN(!(token.type == T_VAR), SYNTAX_ERR)

            name = malloc(sizeof(char));
            strcpy(name, token.value.is_char);
            /* insert variable name into function table */
            inserted = insert_variable(function_symtable, name, TYPE_UNKNOWN);
            IF_RETURN(inserted, SYNTAX_ERR)

            /* insert argument name to list of arguments */
            IF_RETURN(!DLInsertLast(arg_list, name), ERR_INTERNAL)

            countParams++;
            IF_RETURN(get_token(), TOKEN_ERR)
        }

        function->params_quantity = countParams;
        function->list = arg_list;
        function->has_params = true;

        return SYNTAX_OK;

    } /* function without arguments */
    else if (token.type == T_RIGHT_BRACKET) {
        function->params_quantity = 0;
        function->list = NULL;
        function->has_params = false;
        return SYNTAX_OK;
    }

    return SYNTAX_ERR;
}


int recursive_descent(AST_NODE **ast, STACK *indent_stack, tDLList *functions_list)
{

    int result = 0;

    while (1) {
        IF_RETURN(get_token(), TOKEN_ERR)

        int item = OK;
        HTable *function_table = NULL;

        IF_RETURN(!ast_add_node(ast, PROG, NULL, true), ERR_INTERNAL)

        /* end of file */
        if (token.type == T_IS_EOF) {
            // todo ???

        } /* function definition */
        else if (token.type == T_DEF) {
            IF_RETURN(get_token(), TOKEN_ERR)

            /* check if token is identifier */
            IF_RETURN(!is_identifier(token.type), SYNTAX_ERR)

            char *name = malloc(sizeof(char));
            /* save name */
            strcpy(name, token.value.is_char);

            HTItem *found = ht_search(global_hashtable, name);

            /* definition of existing function or using same name is not possible */
            IF_RETURN(found, SEM_ERR_UNDEF_VAR)

            /* create symtable for function */
            function_table = ht_init();
            IF_RETURN(!function_table, ERR_INTERNAL)

            /* add function to global symtable*/
            item = insert_function(global_hashtable, function_table, name, -1, NULL);
            IF_VALUE_RETURN(item)

            /* add node to AST*/
            AST_NODE *function_node = ast_add_node(ast, FUNC_DEF, name, SCOPE);
            IF_RETURN(!function_node, ERR_INTERNAL)

            /* left bracket*/
            IF_RETURN(get_token(), TOKEN_ERR)
            IF_RETURN(!is_left_bracket(token.type), SYNTAX_ERR)

            /* get next token - variable name */
            IF_RETURN(get_token(), TOKEN_ERR)

            /* arguments with right bracket */
            int arg = function_arguments(function_table, name);
            IF_RETURN(arg != SYNTAX_OK, SYNTAX_ERR)

            /* colon */
            IF_RETURN(get_token(), TOKEN_ERR)
            IF_RETURN(token.type != T_IS_COLON, SYNTAX_ERR)

            /* eol */
            IF_RETURN(get_token(), TOKEN_ERR)
            IF_RETURN(!is_eol(token.type), SYNTAX_ERR)

            while (is_eol(token.type)) {
                IF_RETURN(get_token(), TOKEN_ERR)
            }

            /* INDENT */
            /* indent has to be greater */
            IF_RETURN(indent_stack->top->indent_counter >= indent_counter, SYNTAX_ERR )

            /* add body of function to ast*/
            AST_NODE *function_body = ast_add_node(&function_node, BODY, NULL, SCOPE);
            IF_RETURN(!function_body, ERR_INTERNAL)

            /* statement list - body of function */
            result = statement_list(SCOPE, function_table, &function_body, indent_stack, functions_list);

        } /* variable */
        else if (token.type == T_VAR) {
            result = statement(GLOBAL_SCOPE, global_hashtable, ast, indent_stack, functions_list);

        } /* if statement */
        else if (token.type == T_IF) {
            result = statement(GLOBAL_SCOPE, global_hashtable, ast, indent_stack, functions_list);

        } /* while */
        else if (token.type == T_WHILE) {
            result = statement(GLOBAL_SCOPE, global_hashtable, ast, indent_stack, functions_list);

        } /* pass */
        else if (token.type == T_PASS) {
            result = statement(GLOBAL_SCOPE, global_hashtable, ast, indent_stack, functions_list);

        }
        else {
            result = token.type == T_IS_EOL ? SYNTAX_OK : SYNTAX_ERR;
        }

        IF_VALUE_RETURN(result)
    }
}


int statement_list(int scope, HTable *table, AST_NODE **ast, STACK *indent_stack, tDLList *functions_list)
{
    /* FUNCTION BODY */
    int result = 0;

    /* indent of function definition */
    int initial_indent = indent_stack->top->indent_counter;
    IF_RETURN((stack_push_indent(indent_stack, indent_counter, T_INDENT)), ERR_INTERNAL)

    /* function definition */
    while (indent_stack->top->indent_counter != initial_indent) {
        if (token.type == T_IF || token.type == T_WHILE
            || token.type == T_VAR || token.type == T_RETURN || token.type == T_PASS)
            result = statement(scope, table, ast, indent_stack, functions_list);

        IF_VALUE_RETURN(result)

        /* eol */
        if (is_eol(token.type)) {
            /* check indent */
            while (is_eol(token.type)) {
                IF_RETURN(get_token(), TOKEN_ERR)
            }

            if (indent_stack->top->indent_counter == initial_indent) {
                unget_token();

            } else {
                if (indent_counter < indent_stack->top->indent_counter) {
                    while (indent_stack->top->indent_counter != indent_counter) {
                        stack_pop(indent_stack);
                        /* wrong indent */
                        IF_RETURN((indent_stack->top->indent_counter != indent_counter)
                                  && (indent_stack->top->indent_counter == 0), SYNTAX_ERR)
                    }
                } else if (indent_counter > indent_stack->top->indent_counter) {
                    /* indent is not possible here */
                    return SYNTAX_ERR;
                }
            }

        }

    }

    return OK;

}

int statement(int scope, HTable *table, AST_NODE **ast, STACK *indent_stack, tDLList *functions_list)
{
    int result = 0;

    /* stack init */
    STACK *stack = malloc(sizeof(STACK));
    stack_init(stack);

    TType previous_token = T_UNKNOWN;
    HTItem *found;
    struct TToken *previous_tkn = malloc(sizeof(struct TToken));

    /* variable */
    if (token.type == T_VAR) {
        /* save previous token */
        previous_tkn->type = token.type;
        previous_tkn->value = token.value;

        IF_RETURN(get_token(), TOKEN_ERR)

        /* function call without assignment */
        if (is_left_bracket(token.type)) {

            found = ht_search(global_hashtable, previous_tkn->value.is_char);
            /* function has to be defined if it is called in global scope */
            IF_RETURN(is_global_scope(scope) && !found, SYNTAX_ERR)

            /* in local scope function does not have to defined */
            if (!is_global_scope((scope)) && !found) {
                /* insert function to list for check and suppose it will be defined later */
                IF_RETURN(DLInsertLast(functions_list, previous_tkn->value.is_char), ERR_INTERNAL)
            }

            /* call node to AST */
            struct AST_node *call_node = ast_add_node(ast, CALL, previous_tkn->value.is_char, is_global_scope(scope));
            IF_RETURN(!call_node, ERR_INTERNAL)

            result = function_call(found, table, ast, indent_stack);

        } else {
            AST_NODE *equals = ast_add_node(ast, ASSIGN, NULL, is_global_scope(scope));
            IF_RETURN(!equals, ERR_INTERNAL)

            char *name = malloc(sizeof(name));
            strcpy(name, token.value.is_char);

            /* variable definition if it does not exist */
            found = ht_search(table, name);
            AST_NODE *l_value = found
                                ? ast_add_node(&equals, VAR, name, is_global_scope(scope))
                                : ast_add_node(&equals, VARDEF, name, is_global_scope(scope));
            IF_RETURN(!l_value, ERR_INTERNAL)
            /* save name for later */
            l_value->data = name;

            /* assignment */
            IF_RETURN(get_token(), TOKEN_ERR)

            if (token.type == T_ASSIGNMENT) {
                IF_RETURN(get_token(), TOKEN_ERR)
                result = expression(scope, stack, table, &l_value, name, indent_stack, previous_token);

            } else {
                /* undefined variable */
                result = insert_variable(table, name, UNDEFINED);
            }
        }

    } /* if statement */
    else if (token.type == T_IF) {
        /* condition -> expression */
        IF_RETURN(get_token(), TOKEN_ERR)
        AST_NODE *if_node = ast_add_node(ast, IF_NODE, NULL, is_global_scope(scope));
        AST_NODE *condition_node = ast_add_node(&if_node, CONDITION, NULL, is_global_scope(scope));

        int condition = expression(scope, stack, table, &condition_node, NULL, indent_stack, previous_token);
        IF_VALUE_RETURN(condition)

        /* eol */
        IF_RETURN(get_token(), TOKEN_ERR)
        IF_RETURN(!is_eol(token.type), SYNTAX_ERR)

        while (is_eol(token.type)) {
            IF_RETURN(get_token(), TOKEN_ERR)
        }

        AST_NODE *if_body = ast_add_node(&if_node,IF_BODY, NULL, is_global_scope(scope));

        /* INDENT */
        /* indent has to be greater */
        IF_RETURN(indent_stack->top->indent_counter >= indent_counter, SYNTAX_ERR)
        IF_RETURN((stack_push_indent(indent_stack, indent_counter, T_INDENT)), ERR_INTERNAL)

        result = handle_indent(scope, table, &if_body, indent_stack, functions_list);
        IF_VALUE_RETURN(result)

        /* else */
        IF_RETURN(token.type != T_ELSE, LEX_ERR) /* lex error because of wrong indent */
        AST_NODE *else_body = ast_add_node(&if_node,ELSE_BODY, NULL, is_global_scope(scope));

        /* colon */
        IF_RETURN(get_token(), TOKEN_ERR)
        IF_RETURN(token.type != T_IS_COLON, SYNTAX_ERR)

        /* eol */
        IF_RETURN(get_token(), TOKEN_ERR)
        IF_RETURN(!is_eol(token.type), SYNTAX_ERR)
        while (is_eol(token.type)) {
            IF_RETURN(get_token(), TOKEN_ERR)
        }

        /* INDENT */
        /* indent has to be greater */
        IF_RETURN(indent_stack->top->indent_counter >= indent_counter, SYNTAX_ERR)
        IF_RETURN((stack_push_indent(indent_stack, indent_counter, T_INDENT)), ERR_INTERNAL)

        result = handle_indent(scope, table, &else_body, indent_stack, functions_list);

    } /* while */
    else if (token.type == T_WHILE) {
        AST_NODE *while_node = ast_add_node(ast, WHILE_NODE, NULL, is_global_scope(scope));

        IF_RETURN(get_token(), TOKEN_ERR)
        int condition = expression(scope, stack, table, &while_node, NULL, indent_stack, previous_token);
        IF_VALUE_RETURN(condition)

        /* eol */
        IF_RETURN(get_token(), TOKEN_ERR)
        IF_RETURN(!is_eol(token.type), SYNTAX_ERR)
        while (is_eol(token.type)) {
            IF_RETURN(get_token(), TOKEN_ERR)
        }

        AST_NODE *while_body = ast_add_node(&while_node, BODY, NULL, is_global_scope(scope));

        /* INDENT */
        /* indent has to be greater */
        IF_RETURN(indent_stack->top->indent_counter >= indent_counter, SYNTAX_ERR)
        IF_RETURN((stack_push_indent(indent_stack, indent_counter, T_INDENT)), ERR_INTERNAL)

        result = handle_indent(scope, table, &while_body, indent_stack, functions_list);

    } /* return */
    else if (token.type == T_RETURN) {
        /* return used only in functions */
        IF_RETURN(is_global_scope(scope), SYNTAX_ERR)
        AST_NODE *return_node = ast_add_node(ast, RETURN, NULL, is_global_scope(scope));
        previous_token = T_RETURN;

        IF_RETURN(get_token(), TOKEN_ERR)
        result = expression(scope, stack, table, &return_node, NULL, indent_stack, previous_token);

        IF_VALUE_RETURN(result)

        /* local scope ends with return */
        IF_RETURN(indent_stack->top->indent_counter <= indent_counter, SYNTAX_ERR)
        /* indent counter should be smaller here */
        while (indent_stack->top->indent_counter != indent_counter) {
            stack_pop(indent_stack);
            /* wrong indent */
            IF_RETURN((indent_stack->top->indent_counter != indent_counter)
                      && (indent_stack->top->indent_counter == 0), SYNTAX_ERR)
        }

    } /* pass */
    else if (token.type == T_PASS) {
        /* eol */
        IF_RETURN(get_token(), TOKEN_ERR)
        IF_RETURN(!is_eol(token.type), SYNTAX_ERR)
        while (is_eol(token.type)) {
            IF_RETURN(get_token(), TOKEN_ERR)
        }

        /* local scope ends with pass - similar to return */
        if (!is_global_scope(scope)) {
            IF_RETURN(indent_stack->top->indent_counter <= indent_counter, SYNTAX_ERR)
            /* indent counter should be smaller here */
            while (indent_stack->top->indent_counter != indent_counter) {
                stack_pop(indent_stack);
                /* wrong indent */
                IF_RETURN((indent_stack->top->indent_counter != indent_counter)
                          && (indent_stack->top->indent_counter == 0), SYNTAX_ERR)
            }

        }

    }

    return result;
}

int handle_indent(int scope, HTable *table, AST_NODE **node, STACK *indent_stack, tDLList *functions_list)
{
    int result = 0;

    do {
        result = statement(scope, table, node, indent_stack, functions_list);
        /* indent */
        IF_RETURN(get_token(), TOKEN_ERR)
        if (indent_stack->top->indent_counter < indent_counter) {
            /* greater indent only in IF or WHILE */
            IF_RETURN(token.type != T_IF && token.type != T_WHILE, TOKEN_ERR)
            IF_RETURN((stack_push_indent(indent_stack, indent_counter, T_INDENT)), ERR_INTERNAL)
        }
    } while (indent_counter >= indent_stack->top->indent_counter);

    /* indent counter is always smaller here */
    while (indent_stack->top->indent_counter != indent_counter) {
        stack_pop(indent_stack);
        /* wrong indent */
        IF_RETURN((indent_stack->top->indent_counter != indent_counter)
                  && (indent_stack->top->indent_counter == 0), SYNTAX_ERR)
    }

    return result;
}


int expression(int scope, STACK *stack, HTable *table, AST_NODE **ast, char *token_name, STACK *indent_stack, TType previous_token)
{
    if (previous_token != T_RETURN) IF_RETURN(!(is_assignment_correct(token.type)), SYNTAX_ERR)

    int result = SYNTAX_ERR;

    bool fce_call = false;
    struct TToken *previous_tkn = malloc(sizeof(struct TToken));

    if (is_term(token.type)) {
        if (token.type == T_VAR) {
            /* save previous token */
            previous_tkn->type = token.type;
            previous_tkn->value = token.value;

            IF_RETURN(get_token(), TOKEN_ERR)
            HTItem *found;
            /* function call */
            if (is_left_bracket(token.type)) {
                fce_call = true;

                found = ht_search(global_hashtable, previous_tkn->value.is_char);
                /* function has to be defined if it is called in global scope */
                IF_RETURN(is_global_scope(scope) && !found, SYNTAX_ERR)

                /* call node to AST */
                struct AST_node *call_node = ast_add_node(ast, CALL, previous_tkn->value.is_char, is_global_scope(scope));
                IF_RETURN(!call_node, ERR_INTERNAL)

                result = function_call(found, table, ast, indent_stack);

            } else {
                unget_token();
                token.type = previous_tkn->type;
                token.value = previous_tkn->value;
                /* check if identifier exists */
                found = ht_search(table, token.value.is_char);

                if (!found) {
                    if (!is_global_scope(scope)) {
                        /* look for variable in global table */
                        found = ht_search(global_hashtable, token.value.is_char);
                        IF_RETURN(!found, SEM_ERR_UNDEF_VAR)
                    } else {
                        return SEM_ERR_UNDEF_VAR;
                    }
                }
            }
        }

        if (!fce_call) result = psa(scope, stack, *ast, table, token_name, indent_stack);

    } else if (is_eol(token.type)) {

        result = SYNTAX_OK;
    }

    return result;

}

int function_call(HTItem *found, HTable *function_table, AST_NODE **ast, STACK *indent_stack)
{
    int result = 0;

    IF_RETURN(get_token(), TOKEN_ERR)

    if (is_right_bracket(token.type)) {
        return found->params_quantity != 0 ? SEM_ERR_PARAM_NUM : SYNTAX_OK;

    } else {
        // todo pravidlo k function call
        IF_RETURN(!is_term(token.type), SYNTAX_ERR)
        result = function_call_arg(found, function_table, ast, indent_stack);

        return result;
    }
}

int function_call_arg(HTItem *found, HTable *table, AST_NODE **ast, STACK *indent_stack)
{
    tDLList *list_of_arguments = found->list;

    int countParams = 0;

    /* check first argument */
    int result = check_function_arguments(table);
    IF_VALUE_RETURN(result)
    countParams++;

    int ret;
    IF_RETURN(get_token(), TOKEN_ERR)

    while (token.type != T_RIGHT_BRACKET && list_of_arguments->First != NULL) {
        IF_RETURN(is_comma(get_token()), SYNTAX_ERR)

        IF_RETURN(get_token(), TOKEN_ERR)
        ret = check_function_arguments(table);
        IF_RETURN(ret != 0, ret)
        countParams++;

        list_of_arguments->First = list_of_arguments->First->rptr;
    }

    return found->params_quantity != countParams ? SEM_ERR_PARAM_NUM : SEM_OK;
}

int check_function_arguments(HTable *table)
{
    /* simple check if variable exists */
    if (token.type == T_VAR) {
        /* variable used in function call*/
        HTItem *item = ht_search(table, token.value.is_char);
        IF_RETURN(!item, SEM_ERR_UNDEF_VAR)
    }

    return SEM_OK;
}


int psa(int scope, STACK *stack, AST_NODE *node, HTable *table, char *token_name, STACK *indent_stack)
{

    S_ELEM *top;
    PSA_SYMBOL input;
    HTItem *found;
    struct TToken *previous = malloc(sizeof(struct TToken));
    int result = 0;
    int colon_counter = 0;

    IF_RETURN(stack_push(stack, NULL, NULL, PSA_END, TYPE_UNKNOWN) != OK, ERR_INTERNAL)

    top = top_terminal(stack);
    IF_RETURN(!top, ERR_INTERNAL)

    /* current token from input*/
    input = token_to_psa_symbol();

    while (input != PSA_END || top->psa_symbol != PSA_END) {

        /* EOL is PSA_END but colon has to be in IF or WHILE */
        IF_RETURN((node->operation == CONDITION || node->operation == WHILE_NODE) && token.type == T_IS_EOL, SYNTAX_ERR)

        IF_RETURN(!(input <= PSA_END), ERR_INTERNAL)

        switch (psa_table[top->psa_symbol][input]) {
            case '=':
                IF_RETURN(!(stack_push(stack, &token, node, input, TYPE_UNKNOWN) == OK), ERR_INTERNAL)

                previous->type = token.type;
                previous->value.is_char = token.value.is_char;
                previous->value = token.value;

                IF_RETURN(get_token(), TOKEN_ERR)

                /* find identifier */
                if (token.type == T_VAR) {
                    found = ht_search(table, token.value.is_char);
                    if (!found) found = ht_search(table, token.value.is_char);
                    IF_RETURN(!found, SEM_ERR_UNDEF_VAR)
                }

                input = token_to_psa_symbol();
                IF_RETURN(!(input <= PSA_END), ERR_INTERNAL)
                break;
            case '<':
                /* strange shift with '<' on stack */
                IF_RETURN(!(stack_push_handle(stack, top, START_HANDLE) == OK), ERR_INTERNAL)

                /* find identifier before push with correct data type*/
                if (token.type == T_VAR) {
                    found = ht_search(table, token.value.is_char);
                    IF_RETURN(!found, SEM_ERR_UNDEF_VAR)
                    /* push to stack */
                    IF_RETURN(!(stack_push(stack, &token, NULL, input, found->data_type) == OK), ERR_INTERNAL)

                } else {
                    IF_RETURN(!(stack_push(stack, &token, NULL, input, token_to_data_type(token)) == OK), ERR_INTERNAL)
                }

                /* save token used in reduction */
                previous->type = token.type;
                previous->value = token.value;

                IF_RETURN(get_token(), TOKEN_ERR)
                if (token.type == T_IS_COLON && (node->operation == CONDITION || node->operation == WHILE_NODE)) {
                    colon_counter++;
                    IF_RETURN(colon_counter > 1, SYNTAX_ERR)
                }

                break;
            case '>':
                result = reduce(scope, stack, previous);
                IF_RETURN(result != SYNTAX_OK, result)
                break;
            case 0:
                return SYNTAX_ERR;
            default:
                return ERR_INTERNAL;
        }

        top = top_terminal(stack);
        IF_RETURN(!top, ERR_INTERNAL)

        /* current token from input*/
        input = token_to_psa_symbol();
    }

    if (token_name) {
       result = insert_variable(table, token_name, stack->top->type);
       IF_VALUE_RETURN(result)
    }

    node->childs[node->child_count] = stack->top->node;
    node->child_count++;
    stack_destroy(stack);

    return SYNTAX_OK;
}

int reduce(int scope, STACK *stack, struct TToken *previous)
{

    PSA_SYMBOL rule[4] = {END_HANDLE, END_HANDLE, END_HANDLE, END_HANDLE};
    S_ELEM stack_elem[4];

    int rule_index = 0;
    AST_NODE *node = NULL;
    AST_node_type type_of_node;
    struct TToken *original = malloc(sizeof(struct TToken));
    DATA_TYPE type = TYPE_UNKNOWN;

    int result1 = stack_top_rule(stack, rule, stack_elem, previous);
    bool result2 = is_rule(rule, &rule_index);
    if (!result1 && result2) {
        switch (rule_index) {
            case OPERAND_RULE:
                type = stack_elem[0].current_token->type == T_VAR
                        ? stack_elem[0].type
                        : token_to_data_type(*stack_elem[0].current_token);

                original->type = stack_elem[0].current_token->type;
                original->value = stack_elem[0].current_token->value;
                original->value.is_char = stack_elem[0].current_token->value.is_char;
                node = ast_add_node(&node, original->type == T_VAR ? VAR : VAL, create_value(original), is_global_scope(scope));
                break;

            case BRACKET_RULE:
                type = token_to_data_type(*stack_elem[1].current_token);
                original->type = stack_elem[1].current_token->type;
                original->value = stack_elem[1].current_token->value;
                original->value.is_char = stack_elem[1].current_token->value.is_char;
                node = stack_elem[1].node;
                break;

            case MATHEMATICAL_OPERATION_RULE:
                /* in function definition parameter may have unknown type */
                if (stack_elem[0].type != TYPE_UNKNOWN && stack_elem[2].type != TYPE_UNKNOWN) {
                    type = check_data_type(stack_elem[0].type, stack_elem[2].type);
                    IF_RETURN(type == TYPE_UNKNOWN, SEM_ERR_COMPAT)
                }

                /* undefined variable */
                IF_RETURN(stack_elem[0].type == UNDEFINED || stack_elem[2].type == UNDEFINED, SEM_ERR_UNDEF_VAR)

                original = NULL;
                type_of_node = node_type(&stack_elem[1]);
                IF_RETURN(type_of_node == NO_NODE, ERR_INTERNAL)
                node = ast_add_node(&node, type_of_node, NULL, is_global_scope(scope));
                node->childs[0] = stack_elem[0].node;
                node->childs[1] = stack_elem[2].node;
                node->child_count = 2;
                break;

            default:
                return ERR_INTERNAL;
        }

        while (stack->top->psa_symbol != START_HANDLE) {
            stack_pop(stack);
        }

        /* pop start handle */
        stack_pop(stack);

        stack_push(stack, original, node, NON_TERMINAL, type);
        return SYNTAX_OK;

    } else {
        return SYNTAX_ERR;
    }
}

DATA_TYPE check_data_type(DATA_TYPE type1, DATA_TYPE type2)
{
    return type1 == type2 ? type1 : TYPE_UNKNOWN;
}

bool compare_rules(PSA_SYMBOL *rule1, PSA_SYMBOL *rule2)
{
    int i = 0;

    while (rule1[i] != END_HANDLE || rule2[i] != END_HANDLE) {
        if (rule1[i] != rule2[i] || (rule1[i] == END_HANDLE && rule2[i] == END_HANDLE)) {
            return false;
        }
        i++;
    }

    return true;
}

bool is_rule(PSA_SYMBOL *rule, int *rule_index)
{
    for (int i = 0; i < 3; i++) {
        if (compare_rules(rule, psa_rules[i])) {
            *rule_index = i;
            return true;
        }
    }

    return false;
}

char *create_value(struct TToken *current_token)
{
    char *value = malloc(sizeof(char *));
    char *string = NULL;

    if (current_token->type == T_STRING) {
        int i = 0, allocated = 0;
        while (current_token->value.is_char[i] != '\0') {
            if (current_token->value.is_char[i] != 39) {
                string = realloc(string, (size_t) allocated++);
                string[allocated - 1] = current_token->value.is_char[i];
            }
            i++;
        }
    }

    switch (current_token->type) {
        case T_STRING:
            strcpy(value, "string@");
            value = realloc(value, sizeof(string));
            strcat(value, string);
            break;
        case T_INT:
            sprintf(value, "int@%d", current_token->value.is_int);
            break;
        case T_FLOAT:
            sprintf(value, "float@%f", current_token->value.is_float);
            break;
        case T_NONE:
            value = "nil@nil";
            break;
        case T_VAR:
            value = current_token->value.is_char;
            break;
        default:
            value = NULL;
            break;
    }

    return value;
}

DATA_TYPE token_to_data_type(struct TToken current_token)
{
    DATA_TYPE type;
    switch (current_token.type) {
        case T_STRING:
            type = TYPE_STRING;
            break;
        case T_INT:
            type = TYPE_INTEGER;
            break;
        case T_FLOAT:
            type = TYPE_FLOAT;
            break;
        case T_NONE:
            type = TYPE_NIL;
            break;
        case T_VAR:
            type = TYPE_IDENTIFIER;
            break;
        default:
            type = TYPE_UNKNOWN;
            break;
    }

    return type;
}

PSA_SYMBOL token_to_psa_symbol()
{
    PSA_SYMBOL symbol;

    switch (token.type) {
        case T_MUL:
            symbol = PSA_MULTIPLICATION;
            break;
        case T_DIV:
            symbol = PSA_DIVISION;
            break;
        case T_DIV_INT:
            symbol = PSA_DIVISION_INT;
            break;
        case T_ADD:
            symbol = PSA_ADDITION;
            break;
        case T_SUB:
            symbol = PSA_SUBTRACTION;
            break;
        case T_IS_SMALLER:
            symbol = PSA_LESS;
            break;
        case T_IS_SMALLER_OR_EQUAL:
            symbol = PSA_LESSEQUAL;
            break;
        case T_IS_GREATER:
            symbol = PSA_GREATER;
            break;
        case T_IS_GREATER_OR_EQUAL:
            symbol = PSA_GREATEREQUAL;
            break;
        case T_IS_EQUAL:
            symbol = PSA_EQUAL;
            break;
        case T_IS_NOT_EQUAL:
            symbol = PSA_NOTEQUAL;
            break;
        case T_LEFT_BRACKET:
            symbol = PSA_LBRACKET;
            break;
        case T_RIGHT_BRACKET:
            symbol = PSA_RBRACKET;
            break;
        case T_VAR:
        case T_INT:
        case T_FLOAT:
        case T_STRING:
            symbol = PSA_OPERAND;
            break;
        case T_IS_EOL:
        case T_IS_COLON:
            symbol = PSA_END;
            break;
        default:
            symbol = PSA_NONE;
            break;
    }

    return symbol;
}

int main(int argc, char const *argv[])
{
    /* global table init */
    global_hashtable = ht_init();
    IF_RETURN(!global_hashtable, ERR_INTERNAL)

    AST_NODE *ast = NULL;
    ast_init(&ast);

    /* list of undefined functions */
    tDLList *functions_list = DLInitList();
    int result = 0;

    /* BUILT-IN FUNCTIONS */
    /* input-x */
    result = insert_function(global_hashtable, NULL, "inputs", 0, NULL);
    IF_VALUE_RETURN(result)
    result = insert_function(global_hashtable, NULL, "inputi", 0, NULL);
    IF_VALUE_RETURN(result)
    result = insert_function(global_hashtable, NULL, "inputf", 0, NULL);
    IF_VALUE_RETURN(result)

    /* print */
    result = insert_function(global_hashtable, NULL, "print", -1, NULL);
    IF_VALUE_RETURN(result)

    /* len */
    result = insert_function(global_hashtable, NULL, "len", 1, NULL);
    IF_VALUE_RETURN(result)

    /* substr */
    result = insert_function(global_hashtable, NULL, "substr", 3, NULL);
    IF_VALUE_RETURN(result)

    /* ord */
    result = insert_function(global_hashtable, NULL, "ord", 2, NULL);
    IF_VALUE_RETURN(result)

    /* chr */
    result = insert_function(global_hashtable, NULL, "chr", 1, NULL);
    IF_VALUE_RETURN(result)

    /* stack for indent */
    STACK *indent_stack = malloc(sizeof(STACK));
    stack_init(indent_stack);
    IF_RETURN((stack_push_indent(indent_stack, 0, T_UNKNOWN)), ERR_INTERNAL)

    previous_state = START;
    printf("%d",  recursive_descent(&ast, indent_stack, functions_list));

//    result = recursive_descent(&ast, indent_stack, functions_list);
//    if (result == 0) {
//        /* check if all undefined functions in list are defined */
//        tDLElemPtr tmp = NULL;
//        tmp = functions_list->First;
//        HTItem *found;
//
//        while (tmp != NULL) {
//            found = ht_search(global_hashtable, tmp->data);
//            IF_RETURN(!found, SEM_ERR_UNDEF_VAR)
//
//            tmp = tmp->rptr;
//        }
//    }
//
//    return result;
}