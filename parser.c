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

int get_next_token() {
    int result = get_token();

    return result ? TOKEN_ERR : 0;
}


int function_arguments(HTable *function_symtable, HTItem *new_item) {

    tDLList *arg_list = NULL;
    arg_list = DLInitList();

    int countParams = 0;
    if (is_left_bracket(token.type)) {
        return SYNTAX_OK;

    /* function has arguments */
    } else if (token.type == T_VAR) {

        // todo redefinicia + datovy typ argumentu funkcie
        char *name = malloc(sizeof(char));
        strcpy(name, token.name);
        HTItem *inserted = insert_variable(function_symtable, name, TYPE_UNKNOWN);
        IF_RETURN(!inserted, SYNTAX_ERR)

        /* insert argument name to list of arguments*/
        IF_RETURN(!DLInsertLast(arg_list, name), ERR_INTERNAL)
        new_item->has_params = true;

        /* quantity of params */
        countParams++;

        IF_RETURN(get_next_token(), TOKEN_ERR)

        while (token.type != T_RIGHT_BRACKET) {
            IF_RETURN(!is_comma(token.type), SYNTAX_ERR)
            IF_RETURN(get_next_token(), TOKEN_ERR)

            IF_RETURN(!(token.type == T_VAR), SYNTAX_ERR)

            strcpy(name, token.name);
            /* insert argument to table of function*/
            inserted = insert_variable(function_symtable, name, TYPE_UNKNOWN);
            IF_RETURN(!inserted, SYNTAX_ERR)

            /* insert argument name to list of arguments*/
            IF_RETURN(!DLInsertLast(arg_list, name), ERR_INTERNAL)

            countParams++;
            IF_RETURN(get_next_token(), TOKEN_ERR)
        }

        new_item->params_quantity = countParams;
        new_item->list = arg_list;
        return SYNTAX_OK;

    } // function without arguments
    else if (token.type == T_RIGHT_BRACKET) {
        new_item->params_quantity = 0;
        new_item->list = NULL;
        return SYNTAX_OK;
    }

    return SYNTAX_ERR;
}


int recursive_descent(AST_NODE **ast) {

    IF_RETURN(get_next_token(), TOKEN_ERR)
    HTItem *item = NULL;
    HTable *function_table = NULL;
    HTItem *variable = malloc(sizeof(HTItem));
    int result = 0;

    /* stack init */
    STACK *stack = malloc(sizeof(STACK));
    stack_init(stack);

    IF_RETURN(!ast_add_node(ast, PROG, NULL, true), ERR_INTERNAL)

    // end of file
    if (token.type == T_IS_EOF) {  // rule 1 -> 2 || 5
        return counter ? SYNTAX_ERR : SYNTAX_OK;

    } /* function definition - rule 4 */
    else if (token.type == T_DEF) {
        IF_RETURN(get_next_token(), TOKEN_ERR)
        IF_RETURN(!is_identifier(token.type), SYNTAX_ERR)

        char *name = malloc(sizeof(char));
        strcpy(name, token.name);

        HTItem *found = ht_search(global_hashtable, name);

        if (found) {
            /* definition of existing function is not possible */
            // todo navratovy kod
            IF_RETURN(found->type != FUNCTION, SYNTAX_ERR)
        } else {
            /* create symtable for function */
           function_table = ht_init();
           IF_RETURN(!function_table, ERR_INTERNAL)

            /* add function to global symtable*/
            item = insert_function(global_hashtable, name, -1, true, NULL);
        }

        /* add node to AST*/
        AST_NODE *function_node = ast_add_node(ast, FUNC_DEF, name, SCOPE);
        IF_RETURN(!function_node, ERR_INTERNAL)

        /* left bracket*/
        IF_RETURN(get_next_token(), TOKEN_ERR)
        IF_RETURN(!is_left_bracket(token.type), SYNTAX_ERR)

        IF_RETURN(get_next_token(), TOKEN_ERR)
        /* arguments*/
        int arg = function_arguments(function_table, item);
        IF_RETURN(arg != SYNTAX_OK, SYNTAX_ERR)

        /* add body of function to ast*/
        AST_NODE *function_body = ast_add_node(&function_node, BODY, NULL, SCOPE);
        IF_RETURN(!function_body, ERR_INTERNAL)

        /* statement list */
        result = statement_list(SCOPE, function_table, &function_body, stack, variable);

    } /* rule 11 */
    else if (token.type == T_VAR){
        result = statement(GLOBAL_SCOPE, global_hashtable, ast, stack, variable);
    }

    return result;
}

/**
 *
 * @param local_symtable
 * @return
 */
int statement_list(int scope, HTable *table, AST_NODE **ast, STACK *stack, HTItem *variable)
{
    int result = 0;
    if (table != NULL) get_next_token();

    if (token.type == T_END) {
        return SYNTAX_OK;

    } else if (is_eol(token.type)) {
        IF_RETURN(get_next_token(), TOKEN_ERR)
        if (token.type == T_IF || token.type == T_WHILE || token.type == T_VAR)
            result = statement(scope, table, ast, stack, variable);

        return result;
    }

    return SYNTAX_ERR;
}

int statement(int scope, HTable *table, AST_NODE **ast, STACK *stack, HTItem *variable)
{
    int result = 0;
    /* rule 11 */
    if (token.type == T_VAR) {
        AST_NODE *equals = ast_add_node(ast, ASSIGN, NULL, scope);
        IF_RETURN(!equals, ERR_INTERNAL)

        char *name = malloc(sizeof(name));
        strcpy(name, token.name);
        AST_NODE *l_value = ast_add_node(&equals, VAR, name, scope);
        IF_RETURN(!l_value, ERR_INTERNAL)

        /* equals */
        IF_RETURN(get_next_token(), TOKEN_ERR)
        if (token.type == T_ASSIGNMENT) {
            IF_RETURN(get_next_token(), TOKEN_ERR)
            result = expression(scope, stack, table, ast, name, variable);
        } else {
            *variable = *insert_variable(table, name, TYPE_UNKNOWN);
            result = SYNTAX_OK;
        }
    }

    return result;
}

int expression(int scope, STACK *stack, HTable *function_table, AST_NODE **ast, char *token_name, HTItem *variable) {

    IF_RETURN(!(is_assignment_correct(token.type)), SYNTAX_ERR)
    char *tkn_name =malloc(sizeof(char));
    int result = SYNTAX_ERR;

    if (is_function_call(token, tkn_name)) {
        HTItem *found = ht_search(global_hashtable, tkn_name);
        IF_RETURN(!found, SEM_ERR_UNDEF_VAR)

        /* call node to AST */
        struct AST_node *call_node = ast_add_node(ast, CALL, tkn_name, is_global_scope(scope) ? true : false);
        IF_RETURN(!call_node, ERR_INTERNAL)

        // todo LL gramatika ako pravidlo 20
        result = function_call(found, function_table, ast);

    } else if (is_term(token.type)) {
        if (token.type == T_VAR) {
            /* check if identifier exists */
           HTItem *found = ht_search(function_table, token.name);
           IF_RETURN(!found, SEM_ERR_UNDEF_VAR)
        }

        result = psa(scope, stack, *ast, function_table, variable, token_name);

    } else if (is_eol(token.type)) {
        result = SYNTAX_OK;
    }

    return result;

}

int function_call(HTItem *found, HTable *function_table, AST_NODE **ast)
{
    IF_RETURN(get_next_token(), TOKEN_ERR)

    if (is_right_bracket(token.type)) {
        return found->params_quantity != 0 ? SEM_ERR_PARAM_NUM : SYNTAX_OK;

    } else {
        // todo pravidlo k function call
        IF_RETURN(!is_term(token.type), SYNTAX_ERR)
        int result = function_call_arg(found, function_table, ast);

        return result;
    }
}

int function_call_arg(HTItem *found, HTable *table, AST_NODE **ast) {
    tDLList *list_of_arguments = found->list;

    int countParams = 0;

    /* first variable from list of function arguments*/
    HTItem *arg = ht_search(found->symtable, list_of_arguments->First->data);
    IF_RETURN(!arg, ERR_INTERNAL) // todo neviem aky kod

    /* check first argument */
    int result = check_function_arguments(table, arg);
    IF_RETURN(result != 0, result)
    countParams++;

    int ret;
    IF_RETURN(get_next_token(), TOKEN_ERR)
    while (token.type != T_RIGHT_BRACKET && list_of_arguments->First != NULL) {
        IF_RETURN(is_comma(get_next_token()), SYNTAX_ERR)

        IF_RETURN(get_next_token(), TOKEN_ERR)
        ret = check_function_arguments(table, arg);
        IF_RETURN(ret != 0, ret)
        countParams++;

        list_of_arguments->First = list_of_arguments->First->rptr;
    }

    return found->params_quantity != countParams ? SEM_ERR_PARAM_NUM : SEM_OK;
}

int check_function_arguments(HTable *table, HTItem *arg)
{
    if (token.type == T_VAR) {
        /* variable used in function call*/
        HTItem *item = ht_search(table, token.name);
        IF_RETURN(!item, SEM_ERR_UNDEF_VAR)

        /* check data type of variables*/
        IF_RETURN(item->data_type != arg->data_type, SEM_ERR_COMPAT)

        // todo nejake spracovanie

    } else {
        /* check data type of variable*/
        DATA_TYPE type = token_to_data_type(token);
        IF_RETURN(type != arg->data_type, SEM_ERR_COMPAT)

        // todo nejake spracovanie
    }

    return SEM_OK;
}


int psa(int scope, STACK *stack, AST_NODE *node, HTable *table, HTItem *variable, char *token_name) {

    S_ELEM *top;
    PSA_SYMBOL input;
    struct TToken *previous = malloc(sizeof(struct TToken));
    int result = 0;

    IF_RETURN(stack_push(stack, NULL, NULL, PSA_END, TYPE_UNKNOWN) != OK, ERR_INTERNAL)

   do {
        top = top_terminal(stack);

        /* current token from input*/
        input = token_to_psa_symbol();
        IF_RETURN(!(input <= PSA_END), ERR_INTERNAL)

        switch (psa_table[top->psa_symbol][input]) {
            case '=':
                IF_RETURN(!(stack_push(stack, &token, node, input, TYPE_UNKNOWN) == OK), ERR_INTERNAL)

                previous->type = token.type;
                previous->name = token.name;
                previous->value = token.value;

                IF_RETURN(get_next_token(), TOKEN_ERR)

                /* find identifier */
                if (token.type == T_VAR) {
                    HTItem *found = ht_search(table, token.name);
                    if (!found) found = ht_search(global_hashtable, token.name);
                    IF_RETURN(found && found != T_VAR, SEM_ERR_OTHER)
                    IF_RETURN(!found, SEM_ERR_UNDEF_VAR)
                }

                input = token_to_psa_symbol();
                IF_RETURN(!(input <= PSA_END), ERR_INTERNAL)
                break;
            case '<':
                /* strange shift with '<' on stack */
                IF_RETURN(!(stack_push_handle(stack, top, START_HANDLE) == OK), ERR_INTERNAL)

                IF_RETURN(!(stack_push(stack, &token, NULL, input, TYPE_UNKNOWN) == OK), ERR_INTERNAL)

                previous->type = token.type;
                previous->name = token.name;
                previous->value = token.value;

                IF_RETURN(get_next_token(), TOKEN_ERR)

                /* find identifier */
                if (token.type == T_VAR) {
                    HTItem *found = ht_search(table, token.name);
                    if (!found) found = ht_search(global_hashtable, token.name);
                    IF_RETURN(found && found != T_VAR, SEM_ERR_OTHER)
                    IF_RETURN(!found, SEM_ERR_UNDEF_VAR)
                }

                input = token_to_psa_symbol();
                IF_RETURN(!(input <= PSA_END), ERR_INTERNAL)
                break;
            case '>':
                result = reduce(scope, stack, table, previous);
                IF_RETURN(result != SYNTAX_OK, result)
                break;
            case 0:
                break;
            default:
                return ERR_INTERNAL;
        }
    } while (input != PSA_END || top->psa_symbol != PSA_END);

    *variable = *insert_variable(table, token_name, stack->top->type);
    node->childs[node->child_count] = stack->top->node;
    node->child_count++;
    stack_destroy(stack);
    return SYNTAX_OK;
}

int reduce(int scope, STACK *stack, HTable *table, struct TToken *previous)
{

    PSA_SYMBOL rule[4] = {END_HANDLE, END_HANDLE, END_HANDLE, END_HANDLE};
    S_ELEM stack_elem[4];

    int rule_index = 0;
    AST_NODE *node = NULL;
    AST_node_type type_of_node;
    struct TToken *original = malloc(sizeof(struct TToken));
    DATA_TYPE type;

    int result1 = stack_top_rule(stack, rule, stack_elem, previous);
    bool result2 = is_rule(rule, &rule_index);
    if (result1 == 0 && result2) {
        switch (rule_index) {
            case OPERAND_RULE:
                type = token_to_data_type(*stack_elem[0].current_token);
                original->type = stack_elem[0].current_token->type;
                original->value = stack_elem[0].current_token->value;
                original->name = stack_elem[0].current_token->name;
                node = ast_add_node(&node, original->type == T_VAR ? VAR : VAL, create_value(original), is_global_scope(scope) ? true : false);
                break;
            case BRACKET_RULE:
                type = token_to_data_type(*stack_elem[1].current_token);
                original->type = stack_elem[1].current_token->type;
                original->value = stack_elem[1].current_token->value;
                original->name = stack_elem[1].current_token->name;
                node = stack_elem[1].node;
                break;
            case MATHEMATICAL_OPERATION_RULE:
                type = check_data_type(stack_elem[0].type, stack_elem[2].type);
                IF_RETURN(type == TYPE_UNKNOWN, SEM_ERR_COMPAT)
                original = NULL;
                type_of_node = node_type(&stack_elem[1]);
                IF_RETURN(type_of_node == NO_NODE, ERR_INTERNAL)
                node = ast_add_node(&node, type_of_node, NULL, is_global_scope(scope) ? true : false);
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

        /* pop start handle*/
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

void *create_value(struct TToken *current_token)
{
    void *value;

    switch (current_token->type) {
        case T_STRING:
            value = current_token->value.is_char;
            break;
        case T_INT:
            value = &current_token->value.is_int;
            break;
        case T_FLOAT:
            value = &current_token->value.is_float;
            break;
        case T_NIL:
            value = "nil";
            break;
        case T_VAR || T_FCE:
            value = current_token->name;
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
        case T_NIL:
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

PSA_SYMBOL token_to_psa_symbol() {
    PSA_SYMBOL symbol;

    switch (token.type) {
        case T_MUL:
            symbol = PSA_MULTIPLICATION;
            break;
        case T_DIV:
            symbol = PSA_DIVISION;
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
        case T_FCE:
        case T_INT:
        case T_FLOAT:
        case T_STRING:
            symbol = PSA_OPERAND;
            break;
        case T_IS_EOL:
            symbol = PSA_END;
            break;
        default:
            symbol = PSA_NONE;
            break;
    }

    return symbol;
}

int main(int argc, char const *argv[]) {
    /* global table init */
    global_hashtable = ht_init();
    IF_RETURN(!global_hashtable, ERR_INTERNAL)

    AST_NODE *ast = NULL;
    ast_init(&ast);

    printf("%d",  recursive_descent(&ast));
    //return recursive_descent(&ast);
    return 0;
}