/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file generator.h
 * @brief header file for generator.c
 * @author
 * @author
 * @author
 * @author
 */
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "symtable.h"
#ifndef IFJ_GENERATOR_H
#define IFJ_GENERATOR_H

void create_header();
void identify_header(AST_NODE *ast, HTable *table);
int generate(AST_NODE *ast, HTable *table);
int generate_vardef(int is_assign, AST_NODE *ast, HTable *table);
int generate_assign(AST_NODE *ast, HTable *table);
char *get_data_type(HTItem *item);

#endif
