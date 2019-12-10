/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file parser.c
 * @author xhladi23 Martin Hladis
 * @author xprasi06 Matej Prasil
 * @author xvadur04 Martin Vadura
 * @author xsleza20 Alexandra Slezakova
 */

#include "parser.h"


int function_arguments(HTable* function_symtable, char* function_name, Nnode ast, STACK* indent_stack)
{

    tDLList* arg_list = NULL;
    HTItem* function = ht_search(global_hashtable, function_name);
    IF_RETURN(!function, ERR_INTERNAL)

    arg_list = DLInitList();

    int countParams = 0;
    int success = 0;

    /* function has arguments */
    if (token.type == T_VAR) {

        char* name = token.value.is_char;
        /* argument of function cannot have same name as function */
        IF_RETURN(strcmp(name, function_name) == 0, SEM_ERR_UNDEF_VAR)

        /* insert variable name into function table */
        int inserted = insert_variable(function_symtable, name, TYPE_UNKNOWN);
        IF_VALUE_RETURN(inserted)

        /* insert argument name to list of arguments */
        IF_RETURN(!DLInsertLast(arg_list, name), ERR_INTERNAL)

        /* quantity of params */
        countParams++;
        Nnode argv = ast_add_node(&ast, ARGV, NULL, NULL, indent_stack->top->indent_counter);
        ast_add_node(&argv, PARAM, create_value(&token), NULL, indent_stack->top->indent_counter);

        success = get_token();
        IF_VALUE_RETURN(success)

        while (token.type != T_RIGHT_BRACKET) {
            IF_RETURN(!is_comma(token.type), SYNTAX_ERR)

            success = get_token();
            IF_VALUE_RETURN(success)

            IF_RETURN(!(token.type == T_VAR), SYNTAX_ERR)
            ast_add_node(&argv, PARAM, create_value(&token), NULL, indent_stack->top->indent_counter);
            name = token.value.is_char;
            /* argument of function cannot have same name as function */
            IF_RETURN(strcmp(name, function_name) == 0, SEM_ERR_UNDEF_VAR)
            /* insert variable name into function table */
            inserted = insert_variable(function_symtable, name, TYPE_UNKNOWN);
            IF_RETURN(inserted, SYNTAX_ERR)

            /* insert argument name to list of arguments */
            IF_RETURN(!DLInsertLast(arg_list, name), ERR_INTERNAL)

            countParams++;

            success = get_token();
            IF_VALUE_RETURN(success)
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


int recursive_descent(Nnode ast, STACK* indent_stack, tDLList* functions_list)
{

    int result = 0;
    int success = 0;

    IF_RETURN(!ast_add_node((&ast), PROG, NULL, true, indent_stack->top->indent_counter), ERR_INTERNAL)

    /* temporary stack for saving nodes */
    nStack = malloc(sizeof(*nStack));
    Arr_Nstack = malloc(sizeof(*nStack));
    stackInit(nStack);

    int item = OK;

    while (1) {
        NstackPopAll(nStack);
        NstackPopAll(Arr_Nstack);

        success = get_token();
        IF_VALUE_RETURN(success)

        HTable* function_table = NULL;

        /* end of file */
        if (token.type == T_IS_EOF) {
            /* empty file */
            IF_RETURN(iterator == 0, SYNTAX_ERR)
            return result;
        } /* function definition */
        else if (token.type == T_DEF) {
            IF_VALUE_RETURN(get_token())

            /* check if token is identifier */
            IF_RETURN(!is_identifier(token.type), SYNTAX_ERR)

            /* save name */
            char* name = token.value.is_char;

            HTItem* found = ht_search(global_hashtable, name);

            /* definition of existing function or using same name is not possible */
            IF_RETURN(found, SEM_ERR_UNDEF_VAR)

            /* create symtable for function */
            function_table = ht_init();
            IF_RETURN(!function_table, ERR_INTERNAL)

            /* add function to global symtable*/
            item = insert_function(global_hashtable, function_table, name, -1, NULL);
            IF_VALUE_RETURN(item)

            /* add node to AST*/
            Nnode function_node = ast_add_node(&ast, FUNC_DEF, name, SCOPE, indent_stack->top->indent_counter);
            IF_RETURN(!function_node, ERR_INTERNAL)

            /* left bracket*/
            IF_VALUE_RETURN(get_token())
            IF_RETURN(!is_left_bracket(token.type), SYNTAX_ERR)

            /* get next token - variable name */
            IF_VALUE_RETURN(get_token())

            /* arguments with right bracket */
            int arg = function_arguments(function_table, name, function_node, indent_stack);
            IF_VALUE_RETURN(arg)

            /* colon */
            IF_VALUE_RETURN(get_token())
            IF_RETURN(token.type != T_IS_COLON, SYNTAX_ERR)

            /* eol */
            IF_VALUE_RETURN(get_token())
            IF_RETURN(!is_eol(token.type), SYNTAX_ERR)


            while (is_eol(token.type)) {
                IF_VALUE_RETURN(get_token())
            }

            /* INDENT */
            /* indent has to be greater */
            IF_RETURN(indent_stack->top->indent_counter >= indent_counter, SYNTAX_ERR)

            /* add body of function to ast*/
            Nnode function_body = ast_add_node(&function_node, BODY, NULL, SCOPE, indent_stack->top->indent_counter);
            IF_RETURN(!function_body, ERR_INTERNAL)

            /* statement list - body of function */
            result = statement_list(SCOPE, function_table, function_body, indent_stack, functions_list);

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
            result = SYNTAX_OK;

            if (!is_eol(token.type)) {
                if (token.type == T_ELSE) {
                    result = SYNTAX_ERR;
                } else {
                    if (is_term(token.type) || is_left_bracket(token.type)) {
                        result = statement(GLOBAL_SCOPE, global_hashtable, ast, indent_stack, functions_list);
                    } else {
                        result = LEX_ERR;
                    }
                }
            }
        }

        IF_VALUE_RETURN(result)
    }
}

int statement_list(int scope, HTable* table, Nnode ast, STACK* indent_stack, tDLList* functions_list)
{
    /* FUNCTION BODY */
    int result = 0;
    int success = 0;

    /* indent of function definition */
    int initial_indent = indent_stack->top->indent_counter;
    IF_RETURN((stack_push_indent(indent_stack, indent_counter, T_INDENT)), ERR_INTERNAL)

    /* function definition */
    while (indent_stack->top->indent_counter != initial_indent) {
        if (token.type == T_IF || token.type == T_WHILE
            || token.type == T_VAR || token.type == T_RETURN || token.type == T_PASS)
            result = statement(scope, table, ast, indent_stack, functions_list);

        IF_VALUE_RETURN(result)

        if (token.type == T_RETURN) {
            success = get_token();
            IF_VALUE_RETURN(success)
        }

        /* eol */
        if (is_eol(token.type)) {
            /* check indent */
            while (is_eol(token.type)) {
                success = get_token();
                IF_VALUE_RETURN(success)
            }

            if (indent_stack->top->indent_counter == initial_indent) {
                unget_token();

            }
            else {
                if (indent_counter < indent_stack->top->indent_counter) {
                    while (indent_stack->top->indent_counter != indent_counter) {
                        stack_pop(indent_stack);
                        /* wrong indent */
                        IF_RETURN((indent_stack->top->indent_counter != indent_counter)
                                  && (indent_stack->top->indent_counter == 0), LEX_ERR)
                    }
                    unget_token();
                }
                else if (indent_counter > indent_stack->top->indent_counter) {
                    /* indent is not possible here */
                    return SYNTAX_ERR;
                }
            }

        }

    }

    return OK;

}

int statement(int scope, HTable* table, Nnode ast, STACK* indent_stack, tDLList* functions_list)
{
    int result = SYNTAX_ERR;
    int success = 0;

    /* stack init */
    STACK* stack = malloc(sizeof(STACK));
    stack_init(stack);

    TType previous_token = T_UNKNOWN;
    HTItem* found;
    struct TToken* previous_tkn = malloc(sizeof(struct TToken));

    /* variable */
    if (token.type == T_VAR) {
        /* save previous token */
        previous_tkn->type = token.type;
        previous_tkn->value = token.value;

        success = get_token();
        IF_VALUE_RETURN(success)

        /* function call without assignment */
        if (is_left_bracket(token.type)) {

            found = ht_search(global_hashtable, previous_tkn->value.is_char);
            /* function has to be defined if it is called in global scope */
            IF_RETURN(is_global_scope(scope) && !found, SEM_ERR_UNDEF_VAR)

            /* in local scope function does not have to defined */
            if (!is_global_scope((scope)) && !found) {
                /* insert function to list for check and suppose it will be defined later */
                IF_RETURN(!DLInsertLast(functions_list, previous_tkn->value.is_char), ERR_INTERNAL)
            }

            /* call node to AST */
            Nnode call_node = ast_add_node(&ast, CALL, previous_tkn->value.is_char, is_global_scope(scope), indent_stack->top->indent_counter);
            IF_RETURN(!call_node, ERR_INTERNAL)


            result = function_call(found, table, call_node, indent_stack);

        }
        else {
            Nnode equals = ast_add_node((&ast), ASSIGN, NULL, is_global_scope(scope), indent_stack->top->indent_counter);
            IF_RETURN(!equals, ERR_INTERNAL)

            char* name = previous_tkn->value.is_char;

            /* variable definition if it does not exist */
            found = ht_search(table, name);
            /* variable cannot have same name as function */
            IF_RETURN(found && found->type != IDENTIFIER, SEM_ERR_UNDEF_VAR)
            if (!found && !is_global_scope(scope)) {
                found = ht_search(global_hashtable, name);
                IF_RETURN(found && found->type != IDENTIFIER, SEM_ERR_UNDEF_VAR)
            }

            Nnode l_value = found
                            ? ast_add_node((&equals), VAR, name, is_global_scope(scope), indent_stack->top->indent_counter)
                            : ast_add_node((&equals), VAR_DEF, name, is_global_scope(scope), indent_stack->top->indent_counter);
            IF_RETURN(!l_value, ERR_INTERNAL)

            if (token.type == T_ASSIGNMENT) {
                /* start of expression */
                success = get_token();
                IF_VALUE_RETURN(success)

                result = expression(scope, stack, table, equals, name, indent_stack, previous_token);

                found = ht_search(table, name);
                if (found == NULL)
                    insert_variable(table, name, TYPE_UNKNOWN);
            }
            else {
                IF_RETURN(!is_eol(token.type), SYNTAX_ERR)
                /* undefined variable */
                result = insert_variable(table, name, UNDEFINED);
            }
        }

    } /* if statement */
    else if (token.type == T_IF) {

        if (indent_counter == 0) indent_stack->top->token_type = T_IF;

        /* condition -> expression */
        success = get_token();
        IF_VALUE_RETURN(success)

        Nnode if_node = ast_add_node((&ast), IF_NODE, NULL, is_global_scope(scope), indent_stack->top->indent_counter);
        Nnode condition_node = ast_add_node(&if_node, COND, NULL, is_global_scope(scope), indent_stack->top->indent_counter);

        int condition = expression(scope, stack, table, condition_node, NULL, indent_stack, previous_token);
        IF_VALUE_RETURN(condition)

        /* eol */
        success = get_token();
        IF_VALUE_RETURN(success)
        IF_RETURN(!is_eol(token.type), SYNTAX_ERR)

        while (is_eol(token.type)) {
            success = get_token();
            IF_VALUE_RETURN(success)
        }

        Nnode if_body = ast_add_node((&if_node), IF_BODY, NULL, is_global_scope(scope), indent_stack->top->indent_counter);

        /* INDENT */
        /* indent has to be greater */
        IF_RETURN(indent_stack->top->indent_counter >= indent_counter, SYNTAX_ERR)

        IF_RETURN((stack_push_indent(indent_stack, indent_counter, T_INDENT)), ERR_INTERNAL)

        result = handle_indent(scope, table, if_body, indent_stack, functions_list);
        IF_VALUE_RETURN(result)

        while (is_eol(token.type)) {
            success = get_token();
            IF_VALUE_RETURN(success)
        }

        /* else */
        IF_RETURN(token.type != T_ELSE, SYNTAX_ERR) /* lex error because of wrong indent */
        Nnode else_body = ast_add_node(&if_node, IF_ELSE, NULL, is_global_scope(scope), indent_stack->top->indent_counter);

        /* colon */
        success = get_token();
        IF_VALUE_RETURN(success)
        IF_RETURN(token.type != T_IS_COLON, SYNTAX_ERR)

        /* eol */
        success = get_token();
        IF_VALUE_RETURN(success)

        IF_RETURN(!is_eol(token.type), SYNTAX_ERR)
        while (is_eol(token.type)) {
            success = get_token();
            IF_VALUE_RETURN(success)
        }

        /* INDENT */
        /* indent has to be greater */
        IF_RETURN(indent_stack->top->indent_counter >= indent_counter, SYNTAX_ERR)
        IF_RETURN((stack_push_indent(indent_stack, indent_counter, T_INDENT)), ERR_INTERNAL)

        result = handle_indent(scope, table, else_body, indent_stack, functions_list);

    } /* while */
    else if (token.type == T_WHILE) {
        if (indent_counter == 0) indent_stack->top->token_type = T_WHILE;

        Nnode while_node = ast_add_node(&ast, WHILE, NULL, is_global_scope(scope), indent_stack->top->indent_counter);
        //while_node->data->child_count++;

        success = get_token();
        IF_VALUE_RETURN(success)

        Nnode cond_node = ast_add_node(&while_node, COND, NULL, is_global_scope(scope), indent_stack->top->indent_counter);

        int condition = expression(scope, stack, table, cond_node, NULL, indent_stack, previous_token);
        IF_VALUE_RETURN(condition)

        /* eol */
        success = get_token();
        IF_VALUE_RETURN(success)

        IF_RETURN(!is_eol(token.type), SYNTAX_ERR)
        while (is_eol(token.type)) {
            success = get_token();
            IF_VALUE_RETURN(success)
        }

        Nnode while_body = ast_add_node((&while_node), BODY, NULL, is_global_scope(scope), indent_stack->top->indent_counter);

        /* INDENT */
        /* indent has to be greater */
        if (indent_stack->top->indent_counter >= indent_counter) {
            return LEX_ERR;
        }

        IF_RETURN((stack_push_indent(indent_stack, indent_counter, T_INDENT)), ERR_INTERNAL)

        result = handle_indent(scope, table, while_body, indent_stack, functions_list);

    } /* return */
    else if (token.type == T_RETURN) {
        /* return used only in functions */
        IF_RETURN(is_global_scope(scope), SYNTAX_ERR)
        Nnode return_node = ast_add_node((&ast), RETURN, NULL, is_global_scope(scope), indent_stack->top->indent_counter);
        previous_token = T_RETURN;

        //ast_add_node(&return_node, PARAM, create_value(&token),is_global_scope(scope),indent_stack->top->indent_counter);

        success = get_token();
        IF_VALUE_RETURN(success)

        result = expression(scope, stack, table, return_node, NULL, indent_stack, previous_token);

        IF_VALUE_RETURN(result)

        success = get_token();
        IF_VALUE_RETURN(success)

        /* local scope ends with return */
        IF_RETURN(indent_stack->top->indent_counter <= indent_counter, SYNTAX_ERR)

        while (is_eol(token.type)) {
            success = get_token();
            IF_VALUE_RETURN(success)
        }

        /* indent counter should be smaller here */
        while (indent_stack->top->indent_counter != indent_counter) {
            stack_pop(indent_stack);
            /* wrong indent */
            IF_RETURN((indent_stack->top->indent_counter != indent_counter)
                      && (indent_stack->top->indent_counter == 0), SYNTAX_ERR)
        }

        unget_token();
        for (int i = 0; i < indent_counter; i++) {
            ungetc(32, stdin);
        }

    } /* pass */
    else if (token.type == T_PASS) {
        /* eol */
        success = get_token();
        IF_VALUE_RETURN(success)

        IF_RETURN(!is_eol(token.type), SYNTAX_ERR)

        while (is_eol(token.type)) {
            success = get_token();
            IF_VALUE_RETURN(success)
        }

        unget_token();
        for (int i = 0; i < indent_counter; i++) {
            ungetc(32, stdin);
        }

        /* local scope ends with pass - similar to return */
        if (!is_global_scope(scope)) {
            IF_RETURN(indent_stack->top->indent_counter <= indent_counter, SYNTAX_ERR)

            while (is_eol(token.type)) {
                success = get_token();
                IF_VALUE_RETURN(success)
            }

            /* indent counter should be smaller here */
            while (indent_stack->top->indent_counter != indent_counter) {
                stack_pop(indent_stack);
                /* wrong indent */
                IF_RETURN((indent_stack->top->indent_counter != indent_counter)
                          && (indent_stack->top->indent_counter == 0), SYNTAX_ERR)
            }

        }

        result = SYNTAX_OK;

    }
    else if (token.type == T_IS_EOL || token.type == T_IS_EOF) {
        result = SYNTAX_OK;
    }
    else {
        if (is_term(token.type) || is_left_bracket(token.type)) {
            result = expression(scope, stack, table, NULL, NULL, indent_stack, previous_token);
        } else {
            result = SYNTAX_ERR;
        }
    }

    free(previous_tkn);

    return result;
}

int handle_indent(int scope, HTable* table, Nnode node, STACK* indent_stack, tDLList* functions_list)
{
    int result = 0;
    int help = 1;
    int success = 0;

    do {
        result = statement(scope, table, node, indent_stack, functions_list);
        IF_VALUE_RETURN(result)
        /* indent */
        success = get_token();
        IF_VALUE_RETURN(success)
        if (indent_stack->top->indent_counter < indent_counter) {
            return SYNTAX_ERR;
        }
        else if (indent_stack->top->indent_counter > indent_counter) {
            while (is_eol(token.type)) {
                success = get_token();
                IF_VALUE_RETURN(success)
            }

            while (indent_stack->top->indent_counter != indent_counter) {
                stack_pop(indent_stack);
                /* wrong indent */
                /* if and else has to be on the same level */
                if (indent_stack->top->indent_counter > indent_counter && token.type == T_ELSE && indent_stack->top->token_type == T_IF) {
                    return LEX_ERR;
                }
                else if (indent_stack->top->indent_counter < indent_counter && token.type == T_ELSE && indent_stack->top->token_type == T_IF) {
                    return SYNTAX_ERR;
                }

                if (help) {
                    if ((token.type == T_ELSE && indent_stack->top->token_type == T_WHILE) || token.type != T_ELSE) {
                        unget_token();
                        for (int i = 0; i < indent_counter; i++) {
                            ungetc(32, stdin);
                        }
                        help = 0;
                    }
                }
                IF_RETURN((indent_stack->top->indent_counter != indent_counter)
                          && (indent_stack->top->indent_counter == 0), LEX_ERR)
            }

            if (token.type == T_IS_EOF)
                ungetc(c, stdin);
            break;
        }

        if (indent_stack->top->indent_counter == 0
            && indent_stack->top->indent_counter == indent_counter) {
            if (token.type != T_ELSE) {
                unget_token();
            }
            break;
        }

    } while (indent_counter >= indent_stack->top->indent_counter);

    return result;
}


int expression(int scope, STACK* stack, HTable* table, Nnode ast, char* token_name, STACK* indent_stack, TType previous_token)
{
    if (previous_token != T_RETURN) IF_RETURN(!(is_assignment_correct(token.type)), SYNTAX_ERR)

    int result = SYNTAX_ERR;
    int success = 0;

    bool fce_call = false;
    struct TToken* previous_tkn = malloc(sizeof(struct TToken));

    if (is_term(token.type) || is_left_bracket(token.type)) {
        if (token.type == T_VAR) {
            /* save previous token */
            previous_tkn->type = token.type;
            previous_tkn->value = token.value;
            //Nnode l_cond = ast_add_node( (&ast), token.type, NULL, is_global_scope(scope),indent_stack->top->indent_counter);

            success = get_token();
            IF_VALUE_RETURN(success)

            HTItem* found;
            /* function call */
            if (is_left_bracket(token.type)) {
                fce_call = true;

                found = ht_search(global_hashtable, previous_tkn->value.is_char);
                /* function has to be defined if it is called in global scope */
                IF_RETURN(is_global_scope(scope) && !found, SEM_ERR_UNDEF_VAR)

                /* call node to AST */
                Nnode call_node = ast_add_node(&ast, CALL, previous_tkn->value.is_char, is_global_scope(scope), indent_stack->top->indent_counter);
                IF_RETURN(!call_node, ERR_INTERNAL)

                result = function_call(found, table, call_node, indent_stack);

            }
            else {
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
                    }
                    else {
                        return SEM_ERR_UNDEF_VAR;
                    }
                }
            }
        }

        if (!fce_call) result = psa(scope, stack, ast, table, token_name);

    }
    else if (is_eol(token.type)) {

        result = SYNTAX_OK;
    }

    return result;

}

int function_call(HTItem* found, HTable* function_table, Nnode ast, STACK* indent_stack)
{
    int result = 0;
    int success = 0;

    success = get_token();
    IF_VALUE_RETURN(success)

    if (is_right_bracket(token.type)) {
        success = get_token();
        IF_VALUE_RETURN(success)

        return found && found->params_quantity != 0 ? SEM_ERR_PARAM_NUM : SYNTAX_OK;
    }
    else {
        IF_RETURN(!is_term(token.type), SYNTAX_ERR)
        result = function_call_arg(found, function_table, ast, indent_stack);

        if (token.type != T_IS_EOL) {
            success = get_token();
            IF_VALUE_RETURN(success)
        }

        /* return token if function call is used for example in sum*/
        if (token.type != T_IS_EOL)
            unget_token();

        return result;
    }
}

int function_call_arg(HTItem* found, HTable* table, Nnode ast, STACK* indent_stack)
{

    int countParams = 0;
    int success = 0;

    /* check first argument */
    int result = check_function_arguments(table);
    IF_VALUE_RETURN(result)
    countParams++;

    Nnode argv = ast_add_node(&ast, ARGV, NULL, NULL, indent_stack->top->indent_counter);
    if( strstr(create_value(&token),"string@") || strstr(create_value(&token),"float@") || strstr(create_value(&token),"int@") || strstr(create_value(&token),"nil@") )
        ast_add_node(&argv, VAL, create_value(&token), NULL, indent_stack->top->indent_counter);
    else
        ast_add_node(&argv, VAR, create_value(&token), NULL, indent_stack->top->indent_counter);


    int ret;
    success = get_token();
    IF_VALUE_RETURN(success)

    while (token.type != T_RIGHT_BRACKET) {
        IF_RETURN(!is_comma(token.type), SYNTAX_ERR)

        success = get_token();
        IF_VALUE_RETURN(success)

        ret = check_function_arguments(table);
        IF_RETURN(ret != 0, ret)
        countParams++;
        if( strstr(create_value(&token),"string@") || strstr(create_value(&token),"float@") || strstr(create_value(&token),"int@") || strstr(create_value(&token),"nil@") )
            ast_add_node(&argv, VAL, create_value(&token), NULL, indent_stack->top->indent_counter);
        else
            ast_add_node(&argv, VAR, create_value(&token), NULL, indent_stack->top->indent_counter);

        success = get_token();
        IF_VALUE_RETURN(success)

    }

    if (found && found->params_quantity != -1)
        return found->params_quantity != countParams ? SEM_ERR_PARAM_NUM : SEM_OK;
    else
        return SEM_OK;
}

int check_function_arguments(HTable* table)
{
    /* simple check if variable exists */
    if (token.type == T_VAR) {
        /* variable used in function call*/
        HTItem* item = ht_search(table, token.value.is_char);
        IF_RETURN(!item, SEM_ERR_UNDEF_VAR)
    }

    return SEM_OK;
}


int psa(int scope, STACK* stack, Nnode node, HTable* table, char* token_name)
{

    HTItem* found;
    Nnode add_node = NULL;
    S_ELEM* top;
    PSA_SYMBOL input;
    struct TToken* previous = malloc(sizeof(struct TToken));
    int colon_counter = 0;
    int left_bracket = 0;
    int right_bracket = 0;
    int result = 0;
    int success = 0;
    int tmp = 0;

    IF_RETURN(stack_push(stack, PSA_END, TYPE_UNKNOWN) != OK, ERR_INTERNAL)

    top = top_terminal(stack);
    IF_RETURN(!top, ERR_INTERNAL)

    if (token.type == T_LEFT_BRACKET) {
        /* save number of left brackets */
        left_bracket++;

        /* add to ast only if it not unused expression */
        if (node) {
            add_node = ast_add_node((&add_node), LF_BR, "(", is_global_scope(scope), -1);
            stackPush(nStack,add_node);
            tmp = 1;
        }
    }

    /* current token from input*/
    input = token_to_psa_symbol();

    while (input != PSA_END || top->psa_symbol != PSA_END) {

        /* EOL is PSA_END but colon has to be in IF or WHILE */
        IF_RETURN(node && (node->data->ntype == COND || node->data->ntype == WHILE) && token.type == T_IS_EOL, SYNTAX_ERR)

        IF_RETURN(!(input <= PSA_END), SYNTAX_ERR)

        switch (psa_table[top->psa_symbol][input]) {
            case '=':
                IF_RETURN(!(stack_push(stack, input, TYPE_UNKNOWN) == OK), ERR_INTERNAL)
                /* save token */
                previous->type = token.type;
                previous->value.is_char = token.value.is_char;
                previous->value = token.value;

                if (token.type == T_RIGHT_BRACKET) {
                    /* save number of right brackets */
                    right_bracket++;

                    if (node) {
                        add_node = ast_add_node((&add_node), RG_BR, ")", is_global_scope(scope), -1);
                        stackPush(nStack,add_node);
                    }
                }

                success = get_token();
                IF_VALUE_RETURN(success)

                /* any operator must follow after right bracket or colon in if/while*/
                IF_RETURN(!is_operator(token.type) && node && (node->data->ntype == COND || node->data->ntype == WHILE) && token.type != T_IS_COLON, SYNTAX_ERR)

                input = token_to_psa_symbol();
                IF_RETURN(!(input <= PSA_END), ERR_INTERNAL)
                break;
            case '<':
                /* strange shift with '<' on stack */
                IF_RETURN(!(stack_push_handle(stack, top, START_HANDLE) == OK), ERR_INTERNAL)

                /* find identifier before push with correct data type*/
                if (token.type == T_VAR) {
                    found = ht_search(table, token.value.is_char);
                    if (!found) {
                        if (!is_global_scope(scope)) {
                            found = ht_search(global_hashtable, token.value.is_char);
                        }

                        IF_RETURN(!found, SEM_ERR_UNDEF_VAR)
                    }

                    /* check if variable is defined*/
                    IF_RETURN(found->data_type == UNDEFINED, SEM_ERR_UNDEF_VAR)

                    /* push to stack */
                    IF_RETURN(!(stack_push(stack, input, found->data_type) == OK), ERR_INTERNAL)
                }
                else {
                    IF_RETURN(!(stack_push(stack, input, token_to_data_type(token)) == OK), ERR_INTERNAL)
                }

                /* save token used in reduction */
                previous->type = token.type;
                previous->value = token.value;

                /* add to ast only if it not unused expression */
                if (node) {
                    if(token.type == T_ADD)
                        add_node = ast_add_node((&add_node), ADD, NULL, is_global_scope(scope), -1);
                    else if(token.type == T_SUB)
                        add_node = ast_add_node((&add_node), SUB, NULL, is_global_scope(scope), -1);
                    else if(token.type == T_MUL)
                        add_node = ast_add_node((&add_node), MUL, NULL, is_global_scope(scope), -1);
                    else if(token.type == T_DIV)
                        add_node = ast_add_node((&add_node), DIV, NULL, is_global_scope(scope), -1);
                    else if(token.type == T_DIV_INT)
                        add_node = ast_add_node((&add_node), DIVINIT, NULL, is_global_scope(scope), -1);
                    else if(token.type == T_IS_EQUAL)
                        add_node = ast_add_node((&add_node), COMP, NULL, is_global_scope(scope), -1);
                    else if(token.type == T_IS_NOT_EQUAL)
                        add_node = ast_add_node((&add_node), NOTCOMP, NULL, is_global_scope(scope), -1);
                    else if(token.type == T_IS_GREATER)
                        add_node = ast_add_node((&add_node), GR, NULL, is_global_scope(scope), -1);
                    else if(token.type == T_IS_GREATER_OR_EQUAL)
                        add_node = ast_add_node((&add_node), GEQ, NULL, is_global_scope(scope), -1);
                    else if(token.type == T_IS_SMALLER)
                        add_node = ast_add_node((&add_node), LESS, NULL, is_global_scope(scope), -1);
                    else if(token.type == T_IS_SMALLER_OR_EQUAL)
                        add_node = ast_add_node((&add_node), LOQ, NULL, is_global_scope(scope), -1);

                    else if (token.type == T_VAR)
                        add_node = ast_add_node((&add_node), VAR, token.value.is_char, is_global_scope(scope), -1);
                    else if (token.type != T_LEFT_BRACKET && token.type != T_RIGHT_BRACKET)
                        add_node = ast_add_node((&add_node), VAL, create_value(&token), is_global_scope(scope), -1);
                    if(tmp == 0)
                        stackPush(nStack,add_node);
                    tmp = 0;
                }

                success = get_token();
                IF_VALUE_RETURN(success)

                IF_RETURN((previous->type == T_DIV || previous->type == T_DIV_INT) && (token.value.is_int == 0 || token.value.is_float == 0),
                          ERR_ZERO_DIV)

                if (token.type == T_IS_COLON && node && (node->data->ntype == COND || node->data->ntype == WHILE)) {
                    colon_counter++;
                    IF_RETURN(colon_counter > 1, SYNTAX_ERR)
                }


                if (token.type == T_LEFT_BRACKET) {
                    if (node) {
                        add_node = ast_add_node((&add_node), LF_BR, "(", is_global_scope(scope), -1);
                    }
                    left_bracket++;
                }

                if (token.type == T_RIGHT_BRACKET && node)
                    add_node = ast_add_node((&add_node), RG_BR, ")", is_global_scope(scope), -1);


                break;
            case '>':
                result = reduce(stack, previous);
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

    /* check brackets */
    IF_RETURN(left_bracket != right_bracket, SYNTAX_ERR)

    if (node) infix2postfix(nStack,node);

    /* update data type of variable */
    if (token_name) {
        result = insert_variable(table, token_name, stack->top->type);
        IF_VALUE_RETURN(result)
    }

    return SYNTAX_OK;
}

int reduce(STACK* stack, struct TToken* previous)
{

    PSA_SYMBOL rule[4] = { END_HANDLE, END_HANDLE, END_HANDLE, END_HANDLE};
    S_ELEM stack_elem[4];
    Ntype type_of_node;
    DATA_TYPE type = TYPE_UNKNOWN;
    int rule_index = 0;
    int result1 = stack_top_rule(stack, rule, stack_elem, previous);
    bool result2 = is_rule(rule, &rule_index);

    if (!result1 && result2) {
        switch (rule_index) {
            case OPERAND_RULE:
                type = stack_elem[0].current_token->type == T_VAR
                       ? stack_elem[0].type
                       : token_to_data_type(*stack_elem[0].current_token);
                break;

            case BRACKET_RULE:
                type = token_to_data_type(*stack_elem[1].current_token);
                break;

            case MATHEMATICAL_OPERATION_RULE:
                /* in function definition parameter may have unknown type */
                if (stack_elem[0].type != TYPE_UNKNOWN && stack_elem[2].type != TYPE_UNKNOWN) {
                    type = check_data_type(stack_elem[0].type, stack_elem[2].type, stack_elem[1].psa_symbol);
                    IF_RETURN(type == TYPE_UNKNOWN, SEM_ERR_COMPAT)
                }

                /* undefined variable */
                IF_RETURN(stack_elem[0].type == UNDEFINED || stack_elem[2].type == UNDEFINED, SEM_ERR_UNDEF_VAR)

                type_of_node = node_type(&stack_elem[1]);
                IF_RETURN(type_of_node == NO_NODE, ERR_INTERNAL)

                break;

            default:
                return ERR_INTERNAL;
        }

        while (stack->top->psa_symbol != START_HANDLE) {
            stack_pop(stack);
        }

        /* pop start handle */
        stack_pop(stack);

        stack_push(stack, NON_TERMINAL, type);
        return SYNTAX_OK;

    }
    else {
        return SYNTAX_ERR;
    }
}

DATA_TYPE check_data_type(DATA_TYPE type1, DATA_TYPE type2, PSA_SYMBOL symbol)
{
    if (symbol == PSA_EQUAL || symbol == PSA_NOTEQUAL) {
        return type1 == TYPE_NIL ? type1 : type2;
    } else {
        if (type1 == TYPE_FLOAT || type2 == TYPE_FLOAT) {
            if (type1 == TYPE_INTEGER || type2 == TYPE_INTEGER) {
                return symbol != PSA_DIVISION_INT ? TYPE_FLOAT : TYPE_UNKNOWN;
            } else {
                return type1 == type2 ? type1 : TYPE_UNKNOWN;
            }
        } else if (type1 == TYPE_STRING || type2 == TYPE_STRING){
            return symbol == PSA_ADDITION ? TYPE_STRING : TYPE_UNKNOWN;
        } else {
            return type1 == type2 ? type1 : TYPE_UNKNOWN;
        }
    }
}

bool compare_rules(PSA_SYMBOL* rule1, PSA_SYMBOL* rule2)
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

bool is_rule(PSA_SYMBOL* rule, int* rule_index)
{
    for (int i = 0; i < 3; i++) {
        if (compare_rules(rule, psa_rules[i])) {
            *rule_index = i;
            return true;
        }
    }

    return false;
}

char* create_value(struct TToken* current_token)
{
    char* value = malloc(sizeof(char*));
    char* string = NULL;

    if (current_token->type == T_STRING) {
        int i = 0, allocated = 0;
        while (current_token->value.is_char[i] != '\0') {
            /* remove character 39 */
            if (current_token->value.is_char[i] != 39) {
                string = realloc(string, (size_t)allocated++);
                string[allocated - 1] = current_token->value.is_char[i];
            }
            i++;
        }

        if (string) {
            string = realloc(string, (size_t)allocated++);
            string[allocated - 1] = '\0';
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
            sprintf(value, "float@%a", current_token->value.is_float);
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
        case T_NONE:
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

int main()
{
    /* global table init */
    global_hashtable = ht_init();
    IF_RETURN(!global_hashtable, ERR_INTERNAL)

    /* number of lines */
    line_counter = 0;
    /* position of character */
    line_position = 0;
    /* end of line flag */
    eol_flag = 0;

    Nnode ast = NULL;
    ast_init(&ast);

    /* list of undefined functions */
    tDLList* functions_list = DLInitList();
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
    STACK* indent_stack = malloc(sizeof(STACK));
    stack_init(indent_stack);
    IF_RETURN((stack_push_indent(indent_stack, 0, T_UNKNOWN)), ERR_INTERNAL)

    previous_state = START;

    result = recursive_descent(ast, indent_stack, functions_list);
    if (result == 0) {
        /* check if all undefined functions in list are defined */
        tDLElemPtr tmp = NULL;
        tmp = functions_list->First;
        HTItem* found;

        while (tmp != NULL) {
            found = ht_search(global_hashtable, tmp->data);
            IF_RETURN(!found, SEM_ERR_UNDEF_VAR)

            tmp = tmp->rptr;
        }

        generate(root, global_hashtable);
    }
    return result;
}