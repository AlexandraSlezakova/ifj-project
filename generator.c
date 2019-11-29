/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file generator.c
 * @brief code generator
 * @author xhladi23 Martin Hladis
 * @author xprasi06 Matej Prasil
 * @author xvadur04 Martin Vadura
 * @author xsleza20 Alexandra Slezakova
 */

#include "generator.h"

char *token_while = NULL;

int lenHelper(unsigned long long x) {
    if (x >= 1000000000) return 10;
    if (x >= 100000000)  return 9;
    if (x >= 10000000)   return 8;
    if (x >= 1000000)    return 7;
    if (x >= 100000)     return 6;
    if (x >= 10000)      return 5;
    if (x >= 1000)       return 4;
    if (x >= 100)        return 3;
    if (x >= 10)         return 2;
    return 1;
}

char * generate_unique_label()
{
    char prefix = '$';
    static unsigned long long int i = 0;
    int lenght = lenHelper(i);
    char *label = malloc((lenght + 2) * sizeof(char));
    label[0] = prefix;
    sprintf(&label[1], "%llu", i++);
    return label;
}

char * generate_unique_identifier()
{
    char prefix = '%';
    static unsigned long long int i = 0;
    int lenght = lenHelper(i);
    char *ident = malloc((lenght + 2) * sizeof(char));
    ident[0] = prefix;
    sprintf(&ident[1], "%llu", i++);
    return ident;
}

void create_local_frame()
{
    fprintf(stdout, "CREATEFRAME\n");
    fprintf(stdout, "PUSHFRAME\n");
}

void end_frame()
{
    fprintf(stdout, "POPFRAME\n");
}

void create_header()
{
    fprintf(stdout, ".IFJcode19\n");
    fprintf(stdout, "JUMP $$main\n");
}
void gerate_math_aritmetic(Nnode ast, HTable *table)
{
    char *new = generate_unique_identifier();
    char *not_int = generate_unique_label();
    char *not_flout = generate_unique_label();
    char *div_not_0 = generate_unique_label();
    char *div_not_0_0 = generate_unique_label();


    if (ast->children[0]->data->ntype == VAL) {
        save_val_to_variable(ast->children[0]);
    }
    if (ast->children[1]->data->ntype == VAL) {
        save_val_to_variable(ast->children[1]);
    }

    generate_type_control_exit(ast, table);
    fprintf(stdout, "DEFVAR %s\n", new);

    if (ast->data->ntype == DIV) {
        
        fprintf(stdout, "JUMPIFNEQ %s LF@%s string@flout\n", not_flout, ast->children[0]->data->data);
        fprintf(stdout, "JUMPIFNEQ %s LF@%s string@flout\n", not_flout, ast->children[1]->data->data);
        fprintf(stdout, "JUMPIFNEQ %s  LF@%s int@0\n", div_not_0_0, ast->children[1]->data->data);
        fprintf(stdout, "EXIT int@57\n");

        fprintf(stdout, "JUMP %s\n", div_not_0_0);
        fprintf(stdout, "LABEL %s\n", not_flout);
        fprintf(stdout, "EXIT int@4\n");
        fprintf(stdout, "LABEL %s\n", div_not_0_0);

        
        fprintf(stdout, "DIV LF@%s LF@%s LF@%s\n", new, ast->children[0]->data->data, ast->children[1]->data->data);
    } else if (ast->data->ntype == DIVINIT){
        fprintf(stdout, "JUMPIFNEQ %s %s@%s string@int\n", not_int, memory_model(ast), ast->children[0]->data->data);
        fprintf(stdout, "JUMPIFNEQ %s %s@%s string@int\n", not_int, memory_model(ast), ast->children[1]->data->data);
        fprintf(stdout, "JUMPIFNEQ %s  %s@%s int@0\n", div_not_0, memory_model(ast), ast->children[1]->data->data);
        fprintf(stdout, "EXIT int@57\n");

        fprintf(stdout, "JUMP %s\n", div_not_0);
        fprintf(stdout, "LABEL %s\n", not_int);
        fprintf(stdout, "EXIT int@4\n");
        fprintf(stdout, "LABEL %s\n", div_not_0);
    } else { 

    }
   
}

void identify_header(Nnode ast, HTable *table)
{
    switch (ast->data->ntype) {
        case VAR_DEF:
            generate_vardef(false, ast, table);
            break;
        case ASSIGN:
            generate_assign(ast, table);
            break;
        case IF_NODE:
            generate_if(ast, table);
            break;
        case WHILE:
            generate_while(ast, table);
            break;
        case COMP:
            generate_aritmetic(ast, table);
            break;
        case GR:
            generate_aritmetic(ast, table);
            break;
        case LESS:
            generate_aritmetic(ast, table);
            break;
        case DIV:
            generate_aritmetic(ast, table);
            break;
        case LOQ:
            generate_aritmetic(ast, table);
            break;
        case NOTCOMP:
            generate_aritmetic(ast, table);
            break;
        case DIVINIT:
            generate_aritmetic(ast, table);
            break;
        case COND:
            identify_header(ast->children[0], table);
            break;
        default:
            break;
    }
}


char *get_data_type(HTItem *item)
{
    char *type;
    switch (item->data_type) {
        case TYPE_STRING:
            type = "string@";
            break;
        case TYPE_FLOAT:
            type = "float@";
            break;
        case TYPE_IDENTIFIER:
            type = "int@";
            break;
        case TYPE_NIL:
            type = "nil@";
            break;
        default:
            type = NULL;
            break;
    }

    return type;
}

int generate_vardef(int is_assign, Nnode ast, HTable *table)
{
    /* find item in table */
    HTItem *found = is_assign
                    ? ht_search(table, ast->children[0]->data->data)
                    : ht_search(table, ast->data->data);
    //IF_RETURN(!found, ERR_INTERNAL)

    char *type = malloc(sizeof(DATA_TYPE));
    if (found != NULL)
        *type = get_data_type(found);

    if(ast->data->inmain)
    {
        if (found != NULL) {
            fprintf(stdout, "DEFVAR %s@%s\n", memory_model(ast), found->key);

            if (is_assign) {
                //generate_assign(ast, table);
                fprintf(stdout, "MOVE %s@%s %s\n", memory_model(ast), found->key, ast->children[1]->data->data);
            }
        } else {
            fprintf(stdout, "DEFVAR %s@%s\n", memory_model(ast), ast->children[0]->data->data);
        }
    }


    //todo
    /*if (ast->data->inmain) {
        fprintf(stdout, "DEFVAR %s%s\n", type, found->key);

        if (is_assign) {
            *//* assign value to variable on global frame*//*
            fprintf(stdout, "MOVE GF@%s %s\n", found->key, ast->children[1]->data->data);
        } else {
            fprintf(stdout, "MOVE GF@%s nil@nil\n", found->key);
        }
    } else {
        fprintf(stdout, "DEFVAR %s%s\n", type, found->key);

        if (is_assign) {
            *//* assign value to variable on local frame*//*
            fprintf(stdout, "MOVE LF@%s %s\n", found->key, ast->children[1]->data->data);
        } else {
            fprintf(stdout, "MOVE LF@%s nil@nil\n", found->key);
        }
    }*/

    return OK;
}

//funkce pro TF
/*bool function_in_node(Nnode ast, int i)
{
    if(ast->children[i]!= NULL)
    {
        function_in_node(ast->children[i],0);
    }
    if(ast->data->ntype == FUNC_DEF)
        return true;
    if(ast->children[i++]!= NULL)
        function_in_node(ast->children[i],i);
    return false;
}*/

char* memory_model(Nnode ast)
{
    if(ast->parent_node == root || ast->data->indent == 0)
        return "GF";
    else
        return "LF";
}

int generate_assign(Nnode ast, HTable *table)
{   
    Nnode tmp; //= ast->children[1];
    Nnode top = ast->children[1];
    int value = 0;
    if (ast->data->inmain) {
        if (ast->children[0]->data->ntype == VAR_DEF) {
            value = generate_vardef(true, ast, table);
            IF_RETURN(value != OK, ERR_INTERNAL)
        } else {
           
            if (ast->children[1]->children[0] != NULL || ast->children[1]->children[0] != NULL) {
               fprintf(stdout, "CREATEFRAME\n");
               fprintf(stdout, "PUSHFRAME\n");
                Tree:
                tmp = top;
                while ((tmp->children[0]->data->ntype != VAL || tmp->children[1]->data->ntype != VAL) &&
                (tmp->children[0]->data->ntype != VAL || tmp->children[1]->data->ntype != VAL)) {
                    if (tmp->children[0]->data->ntype == VAL || tmp->children[0]->data->ntype == VAR) {
                        tmp = tmp->children[1];
                    } else if (tmp->children[1]->data->ntype == VAL || tmp->children[1]->data->ntype == VAR) {
                        tmp = tmp->children[0];
                    }
                }
                identify_header(tmp, table);
                if (tmp != top)
                goto Tree;

            }
            fprintf(stdout, "MOVE GF@%s %s\n", ast->children[0]->data->data, ast->children[1]->data->data);
        }
    } else {
        fprintf(stdout, "MOVE LF@%s %s\n", ast->children[0]->data->data, ast->children[1]->data->data);
    }
}

void aritmetic_operation(Nnode ast, HTable *table)
{
    switch(ast->data->ntype) {
        case ADD:
            fprintf(stdout, "ADD");
            break;
        case SUB:
            fprintf(stdout, "SUB");
            break;
        case MUL:
            fprintf(stdout, "MUL");
            break;
        case DIV:
            fprintf(stdout, "DIV");
            break;
        case GR:
            fprintf(stdout, "GT");
            break;
        case LESS:
            fprintf(stdout, "LT");
            break;
        case COMP:
            fprintf(stdout, "EQ");
            break;
        case NOTCOMP:
            fprintf(stdout, "NOTCOMP");
            break;
        case GEQ:
            fprintf(stdout, "GT");
            break;
        case LOQ:
            fprintf(stdout, "LT");
            break;
        case DIVINIT:
            fprintf(stdout, "IDIV");
            break;
        default:
            break;
    }
}


void generate_type_control_cmp(Nnode ast, HTable *table)
{
    fprintf(stdout, "CREATEFRAME\n");
    fprintf(stdout, "DEFVAR TF@%%1\n");
    fprintf(stdout, "DEFVAR TF@%%2\n");

    if (ast->data->inmain) {
        fprintf(stdout, "MOVE TF@%%1 GF@%s\n", ast->children[0]->data->data);
        fprintf(stdout, "MOVE TF@%%2 GF@%s\n", ast->children[1]->data->data);
    } else {
        fprintf(stdout, "MOVE TF@%%1 LF@%s\n", ast->children[0]->data->data);
        fprintf(stdout, "MOVE TF@%%2 LF@%s\n", ast->children[1]->data->data);
    }
    
    fprintf(stdout, "CALL $contr_cmp\n");

}

void type_control() //type control of  == !=
{   
    fprintf(stdout, "\n");
    fprintf(stdout, "LABEL $contr_cmp\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@cmp1\n");
    fprintf(stdout, "DEFVAR LF@cmp2\n");
    fprintf(stdout, "DEFVAR LF@%%retval\n");

    fprintf(stdout, "TYPE LF@cmp1 LF@%%1\n");
    fprintf(stdout, "TYPE LF@cmp2 LF@%%2\n");

    fprintf(stdout, "JUMPIFEQ same_type LF@cmp1 LF@cmp2\n");
    fprintf(stdout, "MOVE LF@%%retval bool@false\n");
    fprintf(stdout, "JUMP contr_end\n");
    fprintf(stdout, "LABEL same_type\n");

    fprintf(stdout, "MOVE LF@%%retval bool@true\n");
    fprintf(stdout, "LABEL contr_end\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n\n");

}

void generate_type_control_exit(Nnode ast, HTable *table)
{
    fprintf(stdout, "CREATEFRAME\n");
    fprintf(stdout, "DEFVAR TF@%%1\n");
    fprintf(stdout, "DEFVAR TF@%%2\n");

    if (ast->data->inmain) {
        fprintf(stdout, "MOVE TF@%%1 GF@%s\n", ast->children[0]->data->data);
        fprintf(stdout, "MOVE TF@%%2 GF@%s\n", ast->children[1]->data->data);
    } else {
        fprintf(stdout, "MOVE TF@%%1 LF@%s\n", ast->children[0]->data->data);
        fprintf(stdout, "MOVE TF@%%2 LF@%s\n", ast->children[1]->data->data);
    }

    fprintf(stdout, "CALL $contr_exit\n");

    if (ast->data->inmain) {
        fprintf(stdout, "MOVE GF@%s TF@%%1\n", ast->children[0]->data->data);
        fprintf(stdout, "MOVE GF@%s TF@%%2\n", ast->children[1]->data->data);
    } else {
        fprintf(stdout, "MOVE LF@%s TF@%%1\n", ast->children[0]->data->data);
        fprintf(stdout, "MOVE LF@%s TF@%%2\n", ast->children[1]->data->data);
    }
}

void type_control_exit() //typová kontrola =<>=
{
    fprintf(stdout, "\n");
    fprintf(stdout, "LABEL $contr_exit\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@cmp1\n");
    fprintf(stdout, "DEFVAR LF@cmp2\n");
    fprintf(stdout, "DEFVAR LF@%%retval\n");
    fprintf(stdout, "DEFVAR LF@convert\n");
    fprintf(stdout, "TYPE LF@cmp1 LF@%%1\n");
    fprintf(stdout, "TYPE LF@cmp2 LF@%%2\n");
    fprintf(stdout, "JUMPIFEQ same_type_exit LF@cmp1 LF@cmp2\n");
    fprintf(stdout, "JUMPIFEQ wrong_type LF@cmp1 string@string\n");
    fprintf(stdout, "JUMPIFEQ wrong_type LF@cmp2 string@string\n");
    fprintf(stdout, "JUMPIFEQ convert_first LF@cmp1 string@int\n");
    fprintf(stdout, "MOVE LF@convert LF@cmp2\n");
    fprintf(stdout, "INT2FLOAT LF@%%2 LF@convert\n");
    fprintf(stdout, "JUMP end_control_exit\n");

    fprintf(stdout, "LABEL convert_first\n");
    fprintf(stdout, "MOVE LF@convert LF@%%1\n");
    fprintf(stdout, "INT2FLOAT LF@%%1 LF@convert\n");
    fprintf(stdout, "JUMP end_control_exit\n");

    fprintf(stdout, "LABEL wrong_type\n");
    fprintf(stdout, "MOVE LF@%%retval bool@false\n");
    fprintf(stdout, "EXIT int@4\n");
    fprintf(stdout, "LABEL same_type_exit\n");
    fprintf(stdout, "MOVE LF@%%retval bool@true\n");
    fprintf(stdout, "LABEL end_control_exit\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n\n");
}

void save_val_to_variable(Nnode ast)
{
    char *newvar = generate_unique_identifier();
    if (ast->data->inmain) {
        fprintf(stdout, "DEFVAR GF@%s\n", newvar);
        fprintf(stdout, "MOVE GF@%s %s\n", newvar, ast->data->data);
    }
    else {
        fprintf(stdout, "DEFVAR LF@%s\n");
        fprintf(stdout, "MOVE LF@%s %s\n", newvar, ast->data->data);
    }

    free(ast->data->data);
    ast->data->data = newvar;
}


int generate_aritmetic(Nnode ast, HTable *table)
{

    char *new = generate_unique_identifier();
   // char *contr_type_name = generate_unique_identifier();
    char *div_not_0 = generate_unique_label();
    char *div_not_0_0 = generate_unique_label();
    char *not_int =  generate_unique_label();
    char *not_flout = generate_unique_label();
    
    if (ast->children[0]->data->ntype == VAL)
    {
        save_val_to_variable(ast->children[0]);
    }
    if (ast->children[1]->data->ntype == VAL)
    {
        save_val_to_variable(ast->children[1]);
    }
    free(ast->data->data);
    ast->data->data = new;
    if (ast->data->inmain){
        fprintf(stdout, "DEFVAR GF@%s\n", new);
    } else {
        fprintf(stdout, "DEFVAR LF@%s\n", new);
    }

    if (token_while != NULL)
    {
        fprintf(stdout, "LABEL %s\n", token_while);
    }

    if (ast->data->ntype != COMP && ast->data->ntype != NOTCOMP) {
        // generate_type_control_exit(ast, table, contr_type_name);
    }

    if (ast->data->ntype == DIVINIT) {
        fprintf(stdout, "JUMPIFNEQ %s %s@%s string@int\n", not_int, memory_model(ast), ast->children[0]->data->data);
        fprintf(stdout, "JUMPIFNEQ %s %s@%s string@int\n", not_int, memory_model(ast), ast->children[1]->data->data);
        fprintf(stdout, "JUMPIFNEQ %s  %s@%s int@0\n", div_not_0, memory_model(ast), ast->children[1]->data->data);
        fprintf(stdout, "EXIT int@57\n");

        fprintf(stdout, "JUMP %s\n", div_not_0);
        fprintf(stdout, "LABEL %s\n", not_int);
        fprintf(stdout, "EXIT int@4\n");
        fprintf(stdout, "LABEL %s\n", div_not_0);

    } else if (ast->data->ntype == DIV) {
        fprintf(stdout, "JUMPIFNEQ %s %s@%s string@flout\n", not_flout, memory_model(ast), ast->children[0]->data->data);
        fprintf(stdout, "JUMPIFNEQ %s %s@%s string@flout\n", not_flout, memory_model(ast), ast->children[1]->data->data);
        fprintf(stdout, "JUMPIFNEQ %s  %s@%s int@0\n", div_not_0_0, memory_model(ast), ast->children[1]->data->data);
        fprintf(stdout, "EXIT int@57\n");

        fprintf(stdout, "JUMP %s\n", div_not_0_0);
        fprintf(stdout, "LABEL %s\n", not_flout);
        fprintf(stdout, "EXIT int@4\n");
        fprintf(stdout, "LABEL %s\n", div_not_0_0);

    } else if (ast->data->ntype == ADD || ast->data->ntype == MUL || ast->data->ntype == SUB){


    } else if (ast->data->ntype == COMP || ast->data->ntype == NOTCOMP) {
        generate_type_control_cmp(ast, table);

        if (ast->data->inmain){
            fprintf(stdout, "EQ GF@%s GF@%s GF@%s\n", new, ast->children[0]->data->data, ast->children[1]->data->data);
            fprintf(stdout, "AND GF@%s TF@%%retval GF@%s\n", new, new);
            if (ast->data->ntype == NOTCOMP)
                fprintf(stdout, "NOT GF@%s GF@%s\n", new, new);
        } else {
            fprintf(stdout, "EQ LF@%s LF@%s LF@%s\n", new, ast->children[0]->data->data, ast->children[1]->data->data);
            fprintf(stdout, "AND LF@%s TF@%%retval LF@%s\n",  new, new);
            if (ast->data->ntype == NOTCOMP)
                fprintf(stdout, "NOT GF@%s GF@%s\n", new, new);
        }
    }
    else {
        generate_type_control_exit(ast, table);
        aritmetic_operation(ast, table);

        if (ast->data->inmain){
            fprintf(stdout, " GF@%s GF@%s GF@%s\n", new, ast->children[0]->data->data, ast->children[1]->data->data);
            fprintf(stdout, "AND GF@%s TF@%%retval GF@%s\n", new, new);
            if (ast->data->ntype == LOQ){
                fprintf(stdout, "EQ TF@%%retval GF@%s GF@%s\n", ast->children[0]->data->data, ast->children[1]->data->data);
                fprintf(stdout, "OR GF@%s TF@%%retval GF@%s\n", new, new);
            } else if (ast->data->ntype == GEQ) {
                fprintf(stdout, "EQ TF@%%retval GF@%s GF@%s\n", ast->children[0]->data->data, ast->children[1]->data->data);
                fprintf(stdout, "OR GF@%s TF@%%retval GF@%s\n", new, new);
            }
        } else {
            fprintf(stdout, " LF@%s LF@%s LF@%s\n", new, ast->children[0]->data->data, ast->children[1]->data->data);
            fprintf(stdout, "AND LF@%s TF@%%retval LF@%s\n", new, new);
            if (ast->data->ntype == LOQ){
                fprintf(stdout, "EQ TF@%%retval LF@%s LF@%s\n", ast->children[0]->data->data, ast->children[1]->data->data);
                fprintf(stdout, "OR LF@%s TF@%%retval LF@%s\n", new, new);
            } else if (ast->data->ntype == GEQ) {
                fprintf(stdout, "EQ TF@%%retval LF@%s LF@%s\n", ast->children[0]->data->data, ast->children[1]->data->data);
                fprintf(stdout, "OR LF@%s TF@%%retval LF@%s\n", new, new);
            }
        }
    }

}


int generate_if(Nnode ast, HTable *table)
{
    char *label_else = generate_unique_label();
    char *label_end = generate_unique_label();

    generate(ast->children[0], table);
    if(ast->data->inmain){
        fprintf(stdout, "JUMPIFNEQ %s GF@%s bool@true\n", label_else, ast->children[0]->children[0]->data->data);
    } else {
        fprintf(stdout, "JUMPIFNEQ %s LF@%s bool@true\n", label_else, ast->children[0]->children[0]->data->data);

    }
    generate(ast->children[1], table);
    fprintf(stdout, "JUMP %s\n", label_end);
    fprintf(stdout, "LABEL %s\n", label_else);
    if (ast->children[2] != NULL)
        generate(ast->children[2], table);
    fprintf(stdout, "LABEL %s\n", label_end);

    return 0;
}

int generate_while(Nnode ast,HTable *table)
{
    char *label_start = generate_unique_label();
    char *label_end = generate_unique_label();
    token_while = label_start;
    //fprintf(stdout, "LABEL %s\n", label_start);
    generate(ast->children[0], table);
    if (token_while != NULL)
    {   
        token_while = NULL;
    }
    if (ast->data->inmain) {
        fprintf(stdout, "JUMPIFNEQ %s GF@%s bool@true\n", label_end, ast->children[0]->data->data);
    } else {
        fprintf(stdout, "JUMPIFNEQ %s LF@%s bool@true\n", label_end, ast->children[0]->data->data);
    }
    generate(ast->children[1], table);
    fprintf(stdout, "JUMP %s\n", label_start);
    fprintf(stdout, "LABEL %s\n", label_end);
    return 0;
}

int generate(Nnode ast, HTable *table)
{
    if(ast) {
        if (ast->data->ntype == PROG) {
            create_header();
            
            type_control();
            type_control_exit();
            fprintf(stdout, "LABEL $$main\n");
            create_local_frame();

            //identify_header(ast, table);
        }
        if (ast->data->ntype == PROG || ast->data->ntype == BODY || ast->data->ntype == IF_BODY || ast->data->ntype == IF_ELSE) {
            for (int i = 0; i < ast->data->child_count; i++) {
                generate(ast->children[i], table);
            }
        } else if (ast->data->ntype != PROG) {
            identify_header(ast, table);
        }
        if (ast->data->ntype == PROG) {
            end_frame();
        }

    }
    return 0;
}
