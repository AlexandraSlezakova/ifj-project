/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file generator.c
 * @brief code generator
 * @author
 * @author
 * @author
 * @author
 */
#include "generator.h"
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
    char prefix = '*';
    static unsigned long long int i = 0;
    int lenght = lenHelper(i);
    char *ident = malloc((lenght + 2) * sizeof(char));
    ident[0] = prefix;
    sprintf(&ident[1], "%llu", i++);
    return ident;
}

void create_header()
{
    fprintf(stdout, ".IFJcode19\n");
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
            generate_if(ast,table);
            break;
        case WHILE:
            generate_while(ast,table);
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
    IF_RETURN(!found, ERR_INTERNAL)
    char *type = get_data_type(found);

    if(ast->data->inmain)
    {
        fprintf(stdout, "DEFVAR %s%s\n",memory_model(ast),found->key);

        if(is_assign)
        {
            fprintf(stdout,"MOVE %s%s %s",memory_model(ast),found->key,type);
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
    int value = 0;
    if (ast->data->inmain) {
        if (ast->children[0]->data->ntype == VAR_DEF) {
            value = generate_vardef(true, ast, table);
            IF_RETURN(value != OK, ERR_INTERNAL)
        } else {
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
        default:
            break;
    }
}
void type_control(Nnode ast, HTable *table, char *contr_type_name) //typová kontrola == !=
{
    char *cmp1 = generate_unique_identifier();
    char *cmp2 = generate_unique_identifier();
    char *label_true = generate_unique_label();
    char *label_false = generate_unique_label();
    char *label_end = generate_unique_label();


    if (ast->data->inmain){
       fprintf(stdout, "DEFVAR GF@%s\n", cmp1);
       fprintf(stdout, "DEFVAR GF@%s\n", cmp2);
       fprintf(stdout, "DEFVAR GF@%s\n", contr_type_name);
       fprintf(stdout, "TYPE GF@%s GF@%s\n", cmp1, ast->children[0]->data->data);
       fprintf(stdout, "TYPE GF@%s GF@%s\n", cmp2, ast->children[1]->data->data);
       fprintf(stdout, "JUMPIFEQ %s GF@%s GF@%s\n", label_true, cmp1, cmp2);
       fprintf(stdout, "MOVE GF%s bool@true\n", contr_type_name);
       fprintf(stdout, "LABEL %s\n", label_true);
       fprintf(stdout, "MOVE GF@%s bool@true\n", contr_type_name);
       fprintf(stdout, "LABEL %s\n", label_end);
    } else {
        fprintf(stdout, "DEFVAR LF@%s\n", cmp1);
        fprintf(stdout, "DEFVAR LF@%s\n", cmp2);
        fprintf(stdout, "DEFVAR LF@%s\n", contr_type_name);
        fprintf(stdout, "TYPE LF@%s GF@%s\n", cmp1, ast->children[0]->data->data);
        fprintf(stdout, "TYPE LF@%s GF@%s\n", cmp2, ast->children[1]->data->data);
        fprintf(stdout, "JUMPIFEQ %s LF@%s GF@%s\n", label_true, cmp1, cmp2);
        fprintf(stdout, "MOVE GF%s bool@true\n", contr_type_name);
        fprintf(stdout, "LABEL %s\n", label_true);
        fprintf(stdout, "MOVE LF@%s bool@true\n", contr_type_name);
        fprintf(stdout, "LABEL %s\n", label_end);
    }
}

void generate_type_control_exit(Nnode ast, HTable *table, char *contr_type_name) //typová kontrola =<>=
{
    char *cmp1 = generate_unique_identifier();
    char *cmp2 = generate_unique_identifier();
    char *convert = generate_unique_identifier();

    char *same_type = generate_unique_label();
    char *wrong_type = generate_unique_label();
    char *label_end = generate_unique_label();
    char *convert_first = generate_unique_label();

    if (ast->data->inmain){
        fprintf(stdout, "DEFVAR GF@%s\n", cmp1); //chyba s nil nil chybí asi
        fprintf(stdout, "DEFVAR GF@%s\n", cmp2);
        fprintf(stdout, "DEFVAR GF@%s\n", contr_type_name);
        fprintf(stdout, "TYPE GF@%s GF@%s\n", cmp1, ast->children[0]->data->data);
        fprintf(stdout, "TYPE GF@%s GF@%s\n", cmp2, ast->children[1]->data->data);
        fprintf(stdout, "JUMPIFEQ %s GF@%s GF@%s\n", same_type, cmp1, cmp2);
        fprintf(stdout, "JUMPIFEQ %s GF@%s string@string\n",  wrong_type, cmp1);
        fprintf(stdout, "JUMPIFEQ %s GF@%s string@string\n",  wrong_type, cmp2);
        fprintf(stdout, "JUMPIFEQ %s GF@%s string@int\n", convert_first, cmp1);
        fprintf(stdout, "DEFVAR GF@%s\n", convert);
        fprintf(stdout, "MOVE GF@%s\n GF@%s", convert, ast->children[1]->data->data);
        fprintf(stdout, "INT2FLOAT GF@%s GF@%s\n", ast->children[1]->data->data, convert);
        fprintf(stdout, "JUMP %s\n", label_end);

        fprintf(stdout, "LABEL %s", convert_first);
        fprintf(stdout, "DEFVAR GF@%s\n", convert);
        fprintf(stdout, "MOVE GF@%s\n GF@%s", convert, ast->children[0]->data->data);
        fprintf(stdout, "INT2FLOAT GF@%s GF@%s\n", ast->children[0]->data->data, convert);
        fprintf(stdout, "JUMP %s\n", label_end);

        fprintf(stdout, "LABEL %s\n", wrong_type);
        fprintf(stdout, "EXIT int@4\n");
        fprintf(stdout, "LABEL %s\n", same_type);
        fprintf(stdout, "MOVE GF@%s bool@true\n", contr_type_name);
        fprintf(stdout, "LABEL %s\n", label_end);

        fprintf(stdout, "MOVE GF%s bool@true\n", contr_type_name);
    } else {
        fprintf(stdout, "DEFVAR LF@%s\n", cmp1); //chyba s nil nil chybí asi
        fprintf(stdout, "DEFVAR LF@%s\n", cmp2);
        fprintf(stdout, "DEFVAR LF@%s\n", contr_type_name);
        fprintf(stdout, "TYPE LF@%s LF@%s\n", cmp1, ast->children[0]->data->data);
        fprintf(stdout, "TYPE LF@%s LF@%s\n", cmp2, ast->children[1]->data->data);
        fprintf(stdout, "JUMPIFEQ %s LF@%s LF@%s\n", same_type, cmp1, cmp2);
        fprintf(stdout, "JUMPIFEQ %s LF@%s string@string\n",  wrong_type, cmp1);
        fprintf(stdout, "JUMPIFEQ %s LF@%s string@string\n",  wrong_type, cmp2);
        fprintf(stdout, "JUMPIFEQ %s LF@%s string@int\n", convert_first, cmp1);
        fprintf(stdout, "DEFVAR LF@%s\n", convert);
        fprintf(stdout, "MOVE LF@%s\n LF@%s", convert, ast->children[1]->data->data);
        fprintf(stdout, "INT2FLOAT LF@%s LF@%s\n", ast->children[1]->data->data, convert);
        fprintf(stdout, "JUMP %s\n", label_end);

        fprintf(stdout, "LABEL %s", convert_first);
        fprintf(stdout, "DEFVAR LF@%s\n", convert);
        fprintf(stdout, "MOVE LF@%s\n LF@%s", convert, ast->children[0]->data->data);
        fprintf(stdout, "INT2FLOAT LF@%s LF@%s\n", ast->children[0]->data->data, convert);
        fprintf(stdout, "JUMP %s\n", label_end);

        fprintf(stdout, "LABEL %s\n", wrong_type);
        fprintf(stdout, "EXIT int@4\n");
        fprintf(stdout, "LABEL %s\n", same_type);
        fprintf(stdout, "MOVE LF@%s bool@true\n", contr_type_name);
        fprintf(stdout, "LABEL %s\n", label_end);

        fprintf(stdout, "MOVE LF%s bool@true\n", contr_type_name);
    }

}



int generate_aritmetic(Nnode ast, HTable *table)
{
    char *new = generate_unique_identifier();
    char *contr_type_name = generate_unique_identifier();
    free(ast->data->data);
    ast->data->data = new;
    if (ast->data->inmain) {
        fprintf(stdout, "DEFVAR GF@%s\n", ast->data->data);
        fprintf(stdout, "MOVE GF@%s nil@nil\n", ast->data->data);
    } else {
        fprintf(stdout, "DEFVAR LF@%s\n", ast->data->data);
        fprintf(stdout, "MOVE LF@%s nil@nil\n", ast->data->data);
    }

    if (ast->data->ntype == COMP || ast->data->ntype == NOTCOMP) {
        type_control(ast, table, contr_type_name);
        //aritmetic_operation(ast, table);

        if (ast->data->inmain){
            fprintf(stdout, "EQ GF@%s GF@%s GF@%s\n", ast->data->data, ast->children[0]->data->data, ast->children[1]->data->data);
            fprintf(stdout, "AND GF@%s GF@%s GF@%s", ast->data->data, ast->data->data, contr_type_name);
            if (ast->data->ntype == NOTCOMP)
                fprintf(stdout, "NOT GF@%s GF@%s", ast->data->data, ast->data->data);
        } else {
            fprintf(stdout, "EQ LF@%s LF@%s GF@%s\n", ast->data->data, ast->children[0]->data->data, ast->children[1]->data->data);
            fprintf(stdout, "AND LF@%s LF@%s LF@%s", ast->data->data, ast->data->data, contr_type_name);
            if (ast->data->ntype == NOTCOMP)
                fprintf(stdout, "NOT LF@%s LF@%s", ast->data->data, ast->data->data);
        }
    }
    else {
        type_control(ast, table, contr_type_name);
        aritmetic_operation(ast, table);

        if (ast->data->inmain){
            fprintf(stdout, " GF@%s GF@%s GF@%s\n", ast->data->data, ast->children[0]->data->data, ast->children[1]->data->data);
            fprintf(stdout, "AND GF@%s GF@%s GF@%s\n", ast->data->data, ast->data->data, contr_type_name);
            if (ast->data->ntype == LOQ){
                fprintf(stdout, "EQ GF@%s GF@%s GF%s\n", contr_type_name, ast->children[0]->data->data, ast->children[1]->data->data);
                fprintf(stdout, "OR GF@%s GF@%s GF%s\n", ast->data->data, ast->data->data, contr_type_name);
            } else if (ast->data->ntype == GEQ) {
                fprintf(stdout, "EQ GF@%s GF@%s GF%s\n", contr_type_name, ast->children[0]->data->data, ast->children[1]->data->data);
                fprintf(stdout, "OR GF@%s GF@%s GF%s\n", ast->data->data, ast->data->data, contr_type_name);
            }
        } else {
            fprintf(stdout, " LF@%s LF@%s LF@%s\n", ast->data->data, ast->children[0]->data->data, ast->children[1]->data->data);
            fprintf(stdout, "AND LF@%s LF@%s LF@%s\n", ast->data->data, ast->data->data, contr_type_name);
            if (ast->data->ntype == LOQ){
                fprintf(stdout, "EQ LF@%s LF@%s LF%s\n", contr_type_name, ast->children[0]->data->data, ast->children[1]->data->data);
                fprintf(stdout, "OR LF@%s LF@%s LF%s\n", ast->data->data, ast->data->data, contr_type_name);
            } else if (ast->data->ntype == GEQ) {
                fprintf(stdout, "EQ LF@%s LF@%s LF%s\n", contr_type_name, ast->children[0]->data->data, ast->children[1]->data->data);
                fprintf(stdout, "OR LF@%s LF@%s LF%s\n", ast->data->data, ast->data->data, contr_type_name);
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
        fprintf(stdout, "JUMPIFNEQ %s GF@%s bool@true\n", label_else, ast->children[0]->data);
    } else {
        fprintf(stdout, "JUMPIFNEQ %s LF@%s bool@true\n", label_else, ast->children[0]->data);

    }
    generate(ast->children[1], table);
    fprintf(stdout, "JUMP %s", label_end);
    fprintf("LABEL %s", label_else);
    if (ast->children[2] != NULL)
        generate( ast->children[2], table);
    fprintf("LABEL %s", label_end);


    return 0;
}
int generate_while(Nnode ast,HTable *table)
{
    char *label_start = generate_unique_identifier();
    char *label_end = generate_unique_identifier();

    fprintf(stdout, "LABEL %s\n", label_start);
    generate(ast->children[0], table);
    if (ast->data->inmain) {
        fprintf(stdout, "JUMPIFNEQ %s GF@%s bool@true\n", label_end, ast->children[0]->data);
    } else {
        fprintf(stdout, "JUMPIFNEQ %s GF@%s bool@true\n", label_end, ast->children[0]->data);
    }
    generate(ast->children[1], table);
    fprintf(stdout, "JUMP %s", label_start);
    fprintf(stdout, "LABEL %s", label_end);
    return 0;
}

int generate(Nnode ast, HTable *table)
{
    if(&ast) {
        if (ast->data->ntype == PROG) {
            create_header();
            identify_header(ast, table);
        }
        if (ast->data->ntype == COND)
        {
            identify_header(ast->children[0], table);
        }
    }
    return 0;
}