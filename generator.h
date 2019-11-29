/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file generator.h
 * @brief header file for generator.c
 * @author xhladi23 Martin Hladis
 * @author xprasi06 Matej Prasil
 * @author xvadur04 Martin Vadura
 * @author xsleza20 Alexandra Slezakova
 */
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "symtable.h"
#ifndef IFJ_GENERATOR_H
#define IFJ_GENERATOR_H

void create_header();
void identify_header(Nnode ast, HTable *table);
int generate(Nnode ast, HTable *table);
int generate_vardef(int is_assign, Nnode ast, HTable *table);
int generate_assign(Nnode ast, HTable *table);
char *get_data_type(HTItem *item);
int generate_if(Nnode ast, HTable *table);
int generate_while(Nnode ast,HTable *table);
char* memory_model(Nnode ast);
int generate_aritmetic(Nnode ast, HTable *table);
void generate_math_aritmetic(Nnode ast, HTable *table);
void aritmetic_operation(Nnode ast, HTable *table);
void generate_type_control_cmp(Nnode ast, HTable *table);
void type_control();
void generate_type_control_exit(Nnode ast, HTable *table);
void type_control_exit();
void save_val_to_variable(Nnode ast);
void generate_call(Nnode ast,HTable *table);
void generate_func_def(Nnode ast,HTable *table);
void generate_write_func(Nnode ast, HTable *table);
void generate_read_func(Nnode ast, HTable *table, char *type);
void indetify_call_function(Nnode ast, HTable *table);
int data2int(char *data);
#endif
