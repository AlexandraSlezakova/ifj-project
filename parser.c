#include "parser.h"
#include "scanner.h"

int get_next_token() {
    int result = get_token();

    if (result) {
        exit(ERR_LEX);
    }

    return 0;
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
        HTItem *inserted = insert_variable(function_symtable, name);
        IF_RETURN(!inserted, SYNTAX_ERR)

        /* insert argument name to list of arguments*/
        IF_RETURN(!DLInsertLast(arg_list, name), ERR_INTERNAL)
        new_item->has_params = true;

        /* quantity of params */
        countParams++;

        get_next_token();

        while (token.type != Tright_bracket) {
            IF_RETURN(!is_comma(token.type), SYNTAX_ERR)
            get_next_token();

            IF_RETURN(!(token.type == TID), SYNTAX_ERR)

            strcpy(name, token.name);
            /* insert argument to table of function*/
            inserted = insert_variable(function_symtable, name);
            IF_RETURN(!inserted, SYNTAX_ERR)

            /* insert argument name to list of arguments*/
            IF_RETURN(!DLInsertLast(arg_list, name), ERR_INTERNAL)

            countParams++;
            get_next_token();
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

    get_next_token();
    HTItem *item = NULL;
    HTable *function_table = NULL;

    IF_RETURN(!ast_add_node(ast, PROG, NULL, true), ERR_INTERNAL)

    // end of file
    if (token.type == TEOF) {  // rule 1 -> 2 || 5
        return counter ? SYNTAX_ERR : SYNTAX_OK;

    } /* function definition - rule 4 */
    else if (token.type == Tdef) {
        get_next_token();
        IF_RETURN(!is_identifier(token.type), SYNTAX_ERR)

        char *name = malloc(sizeof(char));
        strcpy(name, token.name);

        HTItem *found = htRead(global_hashtable, name);

        if (found) {
           item = found;
            // todo redefinicia
        } else {
            /* create symtable for function */
           function_table = htInit();
           IF_RETURN(!function_table, ERR_INTERNAL)

            /* add function to global symtable*/
            item = insert_function(function_table, name, -1, true, NULL);
        }

        // todo ast
        /* add node to AST*/
        AST_NODE *function_node = ast_add_node(ast, FUNC_DEF, name, SCOPE);
        IF_RETURN(!function_node, ERR_INTERNAL)

        /* left bracket*/
        get_next_token();
        IF_RETURN(!is_left_bracket(token.type), SYNTAX_ERR)

        get_next_token();
        /* arguments*/
        int arg = function_arguments(function_table, item);
        IF_RETURN(arg != SYNTAX_OK, SYNTAX_ERR)

        /* add body of function to ast*/
        AST_NODE *function_body = ast_add_node(&function_node, BODY, NULL, SCOPE);
        IF_RETURN(!function_body, ERR_INTERNAL)

        /* statement list */
        statement_list(SCOPE, function_table, &function_body);

    } // global scope
    else {
        //recursive_descent(0, token, NULL);
    }
}

/**
 *
 * @param local_symtable
 * @return
 */
int statement_list(int scope, HTable *function_table, AST_NODE **ast)
{
    if (function_table != NULL) get_next_token();

    if (token.type == Tend) {
        return SYNTAX_OK;

    } else if (is_eol(token.type)) {
        get_next_token();
        if (token.type == Tif || token.type == Twhile || token.type == TID)
            statement(scope, function_table, ast);
    }
}

int statement(int scope, HTable *function_table, AST_NODE **ast)
{

    /* rule 11 */
    if (token.type == TID) {
        AST_NODE *equals = ast_add_node(ast, ASSIGN, NULL, scope);
        IF_RETURN(!equals, ERR_INTERNAL)

        char *name = malloc(sizeof(name));
        strcpy(name, token.name);
        AST_NODE *l_value = ast_add_node(&equals, VAR, name, scope);
        IF_RETURN(!l_value, ERR_INTERNAL)

        /* equals */
        get_next_token();
        if (token.type == Tassignment) {
            get_next_token();
            //expression();
        }
    }
}

PSA_SYMBOL token_to_psa_symbol(struct TToken new_token) {
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
    stack_push(stack, NULL, NULL, PSA_END);

    do {
        top = top_terminal(stack);

        get_next_token();
        /* current token from input*/
        input = token_to_psa_symbol(token);
        IF_RETURN(!(input <= PSA_END), ERR_INTERNAL)

        switch (psa_table[top->psa_symbol][input]) {
            case '=':
                IF_RETURN(!(stack_push(stack, &token, node, input) == OK), ERR_INTERNAL)
                get_next_token();
                input = token_to_psa_symbol(token);
                IF_RETURN(!(input <= PSA_END), ERR_INTERNAL)
                break;
            case '<':
                /* strange shift with '<' on stack */
                IF_RETURN(!(stack_push_handle(stack, top, START_HANDLE) == OK), ERR_INTERNAL)

                IF_RETURN(!(stack_push(stack, &token, NULL, input) == OK), ERR_INTERNAL)
                get_next_token();
                input = token_to_psa_symbol(token);
                IF_RETURN(!(input <= PSA_END), ERR_INTERNAL)
                break;
            case '>':
                reduce(stack, function_table);
                break;
            default:
                return ERR_INTERNAL;
        }
    } while (input == PSA_END && top->psa_symbol == PSA_END);

    node->childs[node->child_count] = stack->top->node;
    node->child_count++;
    stack_destroy(stack);
    return SYNTAX_OK;
}

int reduce(STACK *stack, HTable *function_table)
{

    PSA_SYMBOL rule[4];
    S_ELEM stack_elem[4];

    int rule_index = 0;
    AST_NODE *node = NULL;
    AST_node_type type_of_node;
    struct TToken *original = malloc(sizeof(struct TToken));

    if (stack_top_rule(stack, rule, stack_elem) == 0 && is_rule(rule, &rule_index)) {
        switch (rule_index) {
            case OPERAND_RULE:
                original->type = stack_elem[0].current_token->type;
                original->val = stack_elem[0].current_token->val;
                original->name = stack_elem[0].current_token->name;
                node = ast_add_node(&node, original->type == TID ? VAR : VAL, create_value(original), function_table == NULL ? true : false);
                break;
            case BRACKET_RULE:
                original->type = stack_elem[1].current_token->type;
                original->val = stack_elem[1].current_token->val;
                original->name = stack_elem[1].current_token->name;
                node = stack_elem[1].node;
                break;
            case MATHEMATICAL_OPERATION_RULE:
                // todo semantika na typy
                // todo vysledny typ pri matematickej operacii
                original = NULL;
                type_of_node = node_type(&stack_elem[1]);
                IF_RETURN(type_of_node == NO_NODE, ERR_INTERNAL)
                node = ast_add_node(&node, type_of_node, NULL, function_table == NULL ? true : false);
                node->childs[0] = stack_elem[0].node;
                node->childs[1] = stack_elem[1].node;
                node->child_count = 2;
                break;
            default:
                return ERR_INTERNAL;
        }

        for (int i = 0; i < 5; i++) {
            stack_pop(stack);
        }

        stack_push(stack, original, node, NON_TERMINAL);
        return SYNTAX_OK;

    } else {
        return SYNTAX_ERR;
    }
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
    // global table init
    table_init();

    AST_NODE *ast = NULL;
    ast_init(&ast);

    recursive_descent(&ast);

    return 0;
}