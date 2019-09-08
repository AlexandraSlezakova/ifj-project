/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file symtable.h
 * @brief header file of symtable.c
 * @author
 * @author
 * @author
 * @author
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "list.h"
#include "stack.h"

#ifndef SYMTABLE
#define SYMTABLE

/* size of table */
#define SIZE 101

typedef enum {
    FUNCTION,
    IDENTIFIER,
}HTType;


typedef struct hashtable{
    struct HTItem *first[SIZE];
} HTable;


/* item of hashtable */
typedef struct HTItem{
    char *key;
    struct HTItem *next;
    HTable *symtable;
    int params_quantity;
    bool defined;
    HTType type;
    bool has_params;
    tDLList *list;
    DATA_TYPE data_type;
} HTItem;


int createHash(char *key);
HTable * ht_init();
HTItem *ht_search(HTable *table, char *key);
void *ht_insert(HTable *table, HTItem *new_item);
HTItem *insert_function(HTable *symtable, char *key, int params_quantity, bool defined, tDLList *list);
HTItem *insert_variable(HTable *symtable, char *key, DATA_TYPE data_type);

#endif
