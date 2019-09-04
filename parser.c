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
    } else if (token.type == TID) {

        // todo redefinicia
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

        while (token.type != Tright_bracket) {
            IF_RETURN(!is_comma(token.type), SYNTAX_ERR)
            IF_RETURN(get_next_token(), TOKEN_ERR)

            IF_RETURN(!(token.type == TID), SYNTAX_ERR)

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
    else if (token.type == Tright_bracket) {
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
    int result = 0;

    IF_RETURN(!ast_add_node(ast, PROG, NULL, true), ERR_INTERNAL)

    // end of file
    if (token.type == TEOF) {  // rule 1 -> 2 || 5
        return counter ? SYNTAX_ERR : SYNTAX_OK;

    } /* function definition - rule 4 */
    else if (token.type == Tdef) {
        IF_RETURN(get_next_token(), TOKEN_ERR)
        IF_RETURN(!is_identifier(token.type), SYNTAX_ERR)

        char *name = malloc(sizeof(char));
        strcpy(name, token.name);

        HTItem *found = htSearch(global_hashtable, name);

        if (found) {
           item = found;
            // todo redefinicia
        } else {
            /* create symtable for function */
           function_table = htInit();
           IF_RETURN(!function_table, ERR_INTERNAL)

            /* add function to global symtable*/
            item = insert_function(global_hashtable, name, -1, true, NULL);
        }

        // todo ast
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
        STACK *stack = malloc(sizeof(STACK));
        stack_init(stack);

        result = statement_list(SCOPE, function_table, &function_body, stack);

    } // global scope
    else {
        //recursive_descent(0, token, NULL);
    }

    return result;
}

/**
 *
 * @param local_symtable
 * @return
 */
int statement_list(int scope, HTable *function_table, AST_NODE **ast, STACK *stack)
{
    int result = 0;
    if (function_table != NULL) get_next_token();

    if (token.type == Tend) {
        return SYNTAX_OK;

    } else if (is_eol(token.type)) {
        IF_RETURN(get_next_token(), TOKEN_ERR)
        if (token.type == Tif || token.type == Twhile || token.type == TID)
            result = statement(scope, function_table, ast, stack);

        return result;
    }

    return SYNTAX_ERR;
}

int statement(int scope, HTable *function_table, AST_NODE **ast, STACK *stack)
{
    int result = 0;
    /* rule 11 */
    if (token.type == TID) {
        AST_NODE *equals = ast_add_node(ast, ASSIGN, NULL, scope);
        IF_RETURN(!equals, ERR_INTERNAL)

        char *name = malloc(sizeof(name));
        strcpy(name, token.name);
        AST_NODE *l_value = ast_add_node(&equals, VAR, name, scope);
        IF_RETURN(!l_value, ERR_INTERNAL)

        /* equals */
        IF_RETURN(get_next_token(), TOKEN_ERR)
        if (token.type == Tassignment) {
            IF_RETURN(get_next_token(), TOKEN_ERR)
            result = expression(stack, function_table, ast);
        }
    }

    return result;
}

int expression(STACK *stack, HTable *function_table, AST_NODE **ast) {

    IF_RETURN(!(is_assignment_correct(token.type)), SYNTAX_ERR)

    char *token_name =malloc(sizeof(char));
    int result = SYNTAX_ERR;

    if (is_function_call(token, token_name)) {
        HTItem *found = htSearch(global_hashtable, token_name);
        IF_RETURN(!found, SEM_ERR_UNDEF_VAR)

        /* call node to AST */
        struct AST_node *call_node = ast_add_node(ast, CALL, token_name, function_table == NULL ? true : false);
        IF_RETURN(!call_node, ERR_INTERNAL)

        // todo LL gramatika ako pravidlo 20
        result = function_call(found, function_table, ast);

    } else if (is_term(token.type)) {
        if (token.type == TID) {
            /* check if identifier exists */
           HTItem *found = htSearch(function_table, token.name);
           IF_RETURN(!found, SEM_ERR_UNDEF_VAR)
        }

        result = psa(stack, *ast, function_table);

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
    HTItem *arg = htSearch(found->symtable, list_of_arguments->First->data);
    IF_RETURN(!arg, ERR_INTERNAL) // todo neviem aky kod

    /* check first argument */
    int result = check_function_arguments(table, arg);
    IF_RETURN(result != 0, result)
    countParams++;

    int ret;
    IF_RETURN(get_next_token(), TOKEN_ERR)
    while (token.type != Tright_bracket && list_of_arguments->First != NULL) {
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
    if (token.type == TID) {
        /* variable used in function call*/
        HTItem *item = htSearch(table, token.name);
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

DATA_TYPE token_to_data_type(struct TToken current_token)
{
    DATA_TYPE type;
    switch (current_token.type) {
        case TSTRING_VALUE:
            type = TYPE_STRING;
            break;
        case TINT_VALUE:
            type = TYPE_INTEGER;
            break;
        case TFLOAT_VALUE:
            type = TYPE_FLOAT;
            break;
        case Tnil:
            type = TYPE_NIL;
            break;
        case TID:
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
        case Tmul:
            symbol = PSA_MULTIPLICATION;
            break;
        case Tdiv:
            symbol = PSA_DIVISION;
            break;
        case Tadd:
            symbol = PSA_ADDITION;
            break;
        case Tsub:
            symbol = PSA_SUBTRACTION;
            break;
        case Tlesser:
            symbol = PSA_LESS;
            break;
        case Tlesserequal:
            symbol = PSA_LESSEQUAL;
            break;
        case Tgreater:
            symbol = PSA_GREATER;
            break;
        case Tgreaterequal:
            symbol = PSA_GREATEREQUAL;
            break;
        case Tequal:
            symbol = PSA_EQUAL;
            break;
        case Tnotequal:
            symbol = PSA_NOTEQUAL;
            break;
        case Tleft_bracket:
            symbol = PSA_LBRACKET;
            break;
        case Tright_bracket:
            symbol = PSA_RBRACKET;
            break;
        case TID:
        case TIDFnc:
        case TFLOAT_VALUE:
        case TINT_VALUE:
        case TSTRING_VALUE:
            symbol = PSA_OPERAND;
            break;
        case TEOL:
            symbol = PSA_END;
            break;
        default:
            symbol = PSA_NONE;
            break;
    }

    return symbol;
}

int psa(STACK *stack, AST_NODE *node, HTable *function_table) {

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
                previous->val = token.val;

                IF_RETURN(get_next_token(), TOKEN_ERR)

                /* find identifier */
                if (token.type == TID) {
                    HTItem *found = htSearch(function_table, token.name);
                    if (!found) found = htSearch(global_hashtable, token.name);
                    IF_RETURN(found && found != TID, SEM_ERR_OTHER)
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
                previous->val = token.val;

                IF_RETURN(get_next_token(), TOKEN_ERR)

                /* find identifier */
                if (token.type == TID) {
                    HTItem *found = htSearch(function_table, token.name);
                    if (!found) found = htSearch(global_hashtable, token.name);
                    IF_RETURN(found && found != TID, SEM_ERR_OTHER)
                    IF_RETURN(!found, SEM_ERR_UNDEF_VAR)
                }

                input = token_to_psa_symbol();
                IF_RETURN(!(input <= PSA_END), ERR_INTERNAL)
                break;
            case '>':
                result = reduce(stack, function_table, previous);
                IF_RETURN(result != SYNTAX_OK, result)
                break;
            case 0:
                break;
            default:
                return ERR_INTERNAL;
        }
    } while (input != PSA_END || top->psa_symbol != PSA_END);

    node->childs[node->child_count] = stack->top->node;
    node->child_count++;
    stack_destroy(stack);
    return SYNTAX_OK;
}

int reduce(STACK *stack, HTable *function_table, struct TToken *previous)
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
                original->val = stack_elem[0].current_token->val;
                original->name = stack_elem[0].current_token->name;
                node = ast_add_node(&node, original->type == TID ? VAR : VAL, create_value(original), function_table == NULL ? true : false);
                break;
            case BRACKET_RULE:
                type = token_to_data_type(*stack_elem[1].current_token);
                original->type = stack_elem[1].current_token->type;
                original->val = stack_elem[1].current_token->val;
                original->name = stack_elem[1].current_token->name;
                node = stack_elem[1].node;
                break;
            case MATHEMATICAL_OPERATION_RULE:
                type = check_data_type(stack_elem[0].type, stack_elem[2].type);
                IF_RETURN(type == TYPE_UNKNOWN, SEM_ERR_COMPAT)
                original = NULL;
                type_of_node = node_type(&stack_elem[1]);
                IF_RETURN(type_of_node == NO_NODE, ERR_INTERNAL)
                node = ast_add_node(&node, type_of_node, NULL, function_table == NULL ? true : false);
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
        case TSTRING_VALUE:
            value = current_token->val.p_cval;
            break;
        case TINT_VALUE:
            value = &current_token->val.ival;
            break;
        case TFLOAT_VALUE:
            value = &current_token->val.fval;
            break;
        case Tnil:
            value = "nil";
            break;
        case TID || TIDFnc:
            value = current_token->name;
            break;
        default:
            value = NULL;
            break;
    }

    return value;
}

int main(int argc, char const *argv[]) {
    /* global table init */
    table_init();

    AST_NODE *ast = NULL;
    ast_init(&ast);

    printf("%d",  recursive_descent(&ast));
    //return recursive_descent(&ast);
    return 0;
}