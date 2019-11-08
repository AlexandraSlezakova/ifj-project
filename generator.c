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
            ? ht_search(table, ast->nodes[0]->data)
            : ht_search(table, ast->data);
    IF_RETURN(!found, ERR_INTERNAL)
    char *type = get_data_type(found);


    //todo
    if (ast->data->inmain) {
        fprintf(stdout, "DEFVAR %s%s\n", type, found->key);

        if (is_assign) {
            /* assign value to variable on global frame*/
            fprintf(stdout, "MOVE GF@%s %s\n", found->key, ast->nodes[1]->data);
        } else {
            fprintf(stdout, "MOVE GF@%s nil@nil\n", found->key);
        }
    } else {
        fprintf(stdout, "DEFVAR %s%s\n", type, found->key);

        if (is_assign) {
            /* assign value to variable on local frame*/
            fprintf(stdout, "MOVE LF@%s %s\n", found->key, ast->nodes[1]->data);
        } else {
            fprintf(stdout, "MOVE LF@%s nil@nil\n", found->key);
        }
    }

    return OK;
}

int generate_assign(Nnode ast, HTable *table)
{
    int value = 0;
    if (ast->data->inmain) {
        if (ast->nodes[0]->data->ntype == VAR_DEF) {
            value = generate_vardef(true, ast, table);
            IF_RETURN(value != OK, ERR_INTERNAL)
        } else {
            fprintf(stdout, "MOVE GF@%s %s\n", ast->nodes[0]->data, ast->nodes[1]->data);
        }
    } else {
        fprintf(stdout, "MOVE LF@%s %s\n", ast->nodes[0]->data, ast->nodes[1]->data);
    }
}

int generate(Nnode ast, HTable *table)
{
    if (ast) {
        if (ast->data->ntype == PROG) {
            create_header();
            identify_header(ast, table);
        }
    }
    return 0;
}