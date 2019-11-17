/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file parser.h
 * @brief header file for parser.c
 * @author
 * @author
 * @author
 * @author
 */
#include "generator.h"

const char psa_table[15][15] =
{ /*  *    /    //   +    -    <    <=   >    >=   ==   !=   (    )   id    $  */
    {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, /*  *  */
    {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, /*  /  */
    {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, /* //  */
    {'<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, /*  +  */
    {'<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, /*  -  */
    {'<', '<', '<', '<', '<',  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '<', '>', '<', '>'}, /*  <  */
    {'<', '<', '<', '<', '<',  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '<', '>', '<', '>'}, /*  <= */
    {'<', '<', '<', '<', '<',  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '<', '>', '<', '>'}, /*  >  */
    {'<', '<', '<', '<', '<',  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '<', '>', '<', '>'}, /*  >= */
    {'<', '<', '<', '<', '<',  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '<', '>', '<', '>'}, /*  == */
    {'<', '<', '<', '<', '<',  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '<', '>', '<', '>'}, /*  != */
    {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<',  0 }, /*  (  */
    {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>',  0 , '>',  0 , '>'}, /*  )  */
    {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>',  0 , '>',  0 , '>'}, /*  id */
    {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<',  0 , '<',  0 }  /*  $  */
};

typedef enum
{
    OPERAND_RULE,
    BRACKET_RULE,
    MATHEMATICAL_OPERATION_RULE
} PSA_RULE_INDEX;

PSA_SYMBOL psa_rules[3][4] =
{
    {PSA_OPERAND, END_HANDLE, END_HANDLE, END_HANDLE},                  /* E -> i      */
    {PSA_LBRACKET, NON_TERMINAL, PSA_RBRACKET, END_HANDLE},             /* E -> (E)    */
    {NON_TERMINAL, MATHEMATICAL_OPERATION, NON_TERMINAL, END_HANDLE}    /* E -> E op E */
};

static int counter = 0;
HTable *global_hashtable;
int statement(int scope, HTable *table, Nnode ast, STACK *indent_stack, tDLList *functions_list);
int statement_list(int scope, HTable *table, Nnode ast, STACK *indent_stack, tDLList *functions_list);
int reduce(int scope, STACK *stack, struct TToken *previous);
int expression(int scope, STACK *stack, HTable *table, Nnode ast, char *token_name, STACK *indent_stack, TType previous_token);
int function_call(HTItem *found, HTable *function_table, Nnode ast, STACK *indent_stack);
int function_call_arg(HTItem *found, HTable *function_table, Nnode ast, STACK *indent_stack);
int function_arguments(HTable *function_symtable, char *function_name);
int check_function_arguments(HTable *table);
int psa(int scope, STACK *stack, Nnode node, HTable *table, char *token_name, STACK *indent_stack);
int recursive_descent(Nnode ast, STACK *indent_stack, tDLList *functions_list);
int handle_indent(int scope, HTable *table, Nnode node, STACK *indent_stack, tDLList *functions_list);
bool is_rule(PSA_SYMBOL *rule, int *rule_index);
bool compare_rules(PSA_SYMBOL *rule1, PSA_SYMBOL *rule2);
char *create_value(struct TToken *current_token);
PSA_SYMBOL token_to_psa_symbol();
DATA_TYPE token_to_data_type(struct TToken current_token);
DATA_TYPE check_data_type(DATA_TYPE type1, DATA_TYPE type2);