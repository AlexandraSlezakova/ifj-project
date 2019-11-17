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
        case WHILE:
            generate_while(ast,table);
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
            ? ht_search(table, ast->childs[0]->data->data)
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
            fprintf(stdout, "MOVE GF@%s %s\n", found->key, ast->childs[1]->data->data);
        } else {
            fprintf(stdout, "MOVE GF@%s nil@nil\n", found->key);
        }
    } else {
        fprintf(stdout, "DEFVAR %s%s\n", type, found->key);

        if (is_assign) {
            *//* assign value to variable on local frame*//*
            fprintf(stdout, "MOVE LF@%s %s\n", found->key, ast->childs[1]->data->data);
        } else {
            fprintf(stdout, "MOVE LF@%s nil@nil\n", found->key);
        }
    }*/

    return OK;
}

//funkce pro TF
/*bool function_in_node(Nnode ast, int i)
{
    if(ast->childs[i]!= NULL)
    {
        function_in_node(ast->childs[i],0);
    }
    if(ast->data->ntype == FUNC_DEF)
        return true;
    if(ast->childs[i++]!= NULL)
        function_in_node(ast->childs[i],i);
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
        if (ast->childs[0]->data->ntype == VAR_DEF) {
            value = generate_vardef(true, ast, table);
            IF_RETURN(value != OK, ERR_INTERNAL)
        } else {
            fprintf(stdout, "MOVE GF@%s %s\n", ast->childs[0]->data->data, ast->childs[1]->data->data);
        }
    } else {
        fprintf(stdout, "MOVE LF@%s %s\n", ast->childs[0]->data->data, ast->childs[1]->data->data);
    }
}

int generate_if(Nnode ast, HTable *table)
{
    return 0;
}
int generate_while(Nnode ast,HTable *table)
{
    return 0;
}

int generate(Nnode ast, HTable *table)
{
    if(&ast) {
        if (ast->data->ntype == PROG) {
            create_header();
            identify_header(ast, table);
        }
    }
    return 0;
}