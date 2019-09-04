#include "semantic.h"

const char psa_table[14][14] =
{ /*  *    /    +    -    <    <=   >    >=   ==   !=   (    )   id    $  */
    {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, /*  *  */
    {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, /*  /  */
    {'<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, /*  +  */
    {'<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>'}, /*  -  */
    {'<', '<', '<', '<',  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '<', '>', '<', '>'}, /*  <  */
    {'<', '<', '<', '<',  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '<', '>', '<', '>'}, /*  <= */
    {'<', '<', '<', '<',  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '<', '>', '<', '>'}, /*  >  */
    {'<', '<', '<', '<',  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '<', '>', '<', '>'}, /*  >= */
    {'<', '<', '<', '<',  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '<', '>', '<', '>'}, /*  == */
    {'<', '<', '<', '<',  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '<', '>', '<', '>'}, /*  != */
    {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<',  0 }, /*  (  */
    {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>',  0 , '>',  0 , '>'}, /*  )  */
    {'>', '>', '>', '>', '>', '>', '>', '>', '>', '>',  0 , '>',  0 , '>'}, /*  id */
    {'<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<',  0 , '<',  0 }  /*  $  */
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


struct TToken *previousToken;
struct TToken *tmpToken;
static int counter = 0;
int get_next_token();
int statement(int scope, HTable *function_table, AST_NODE **ast, STACK *stack);
int function_definition();
int statement_list(int scope, HTable *function_table, AST_NODE **ast, STACK *stack);
int reduce(STACK *stack, HTable *function_table, struct TToken *previous);
int expression(STACK *stack, HTable *function_table, AST_NODE **ast);
int function_call(HTItem *found, HTable *function_table, AST_NODE **ast);
int function_call_arg(HTItem *found, HTable *function_table, AST_NODE **ast);
int check_function_arguments(HTable *table, HTItem *arg);
int psa(STACK *stack, AST_NODE *node, HTable *function_table);
bool is_rule(PSA_SYMBOL *rule, int *rule_index);
bool compare_rules(PSA_SYMBOL *rule1, PSA_SYMBOL *rule2);
void *create_value(struct TToken *current_token);
PSA_SYMBOL token_to_psa_symbol();
tDLList *functionParams(tDLList *function_param_list, HTable *local_symtable);
DATA_TYPE token_to_data_type(struct TToken current_token);
DATA_TYPE check_data_type(DATA_TYPE type1, DATA_TYPE type2);