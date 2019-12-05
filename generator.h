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

typedef struct FrameVar {
    char *var;
    struct FrameVar *next;

}*FrameVars;





void create_header();
void identify_header(Nnode ast, HTable *table);
int generate(Nnode ast, HTable *table);
int generate_vardef( Nnode ast);
int generate_assign(Nnode ast, HTable *table);
char *get_data_type(HTItem *item);
int generate_if(Nnode ast, HTable *table);
int generate_while(Nnode ast,HTable *table);
char* memory_model(Nnode ast);
int generate_aritmetic(Nnode ast);
void generate_math_aritmetic(Nnode ast);
void aritmetic_operation(Nnode ast);
void generate_type_control_cmp(Nnode ast);
void type_control();
void generate_type_control_exit(Nnode ast);
void type_control_exit();
void save_val_to_variable(Nnode ast);
void generate_call(Nnode ast);
void generate_func_def(Nnode ast,HTable *table);
void indetify_call_function(Nnode ast);
int data2int(char *data);
void generate_return(Nnode ast);
void generate_read_func(Nnode ast, char *type);
void generate_write_func(Nnode ast);
void generate_chr_func();
void generate_len_func();
void generate_substr_func();
#endif
