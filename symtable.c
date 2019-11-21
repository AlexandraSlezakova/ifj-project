/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file symtable.c
 * @brief implementation of symbol table as a hash table
 * @author
 * @author
 * @author
 * @author
 */
#include "symtable.h"
#include <string.h>

/**
 * Hash function
 */
int createHash(char *key){

    int ret = 1;
    int key_length = strlen(key);

    for(int i = 0; i < key_length; i++) {
        ret = ret + key[i];
    }

    return(key_length % SIZE);
}


/**
 * Init table
 */
HTable *ht_init(){

    HTable *symtable = NULL;

    symtable = malloc(sizeof(HTable));
   IF_RETURN(!symtable, NULL)

    /* insert null into table */
    for (int i = 0; i < SIZE; i++) {
        (*symtable).first[i] = NULL;
    }

    return symtable;
}


/**
 * Insert new item
 */
int ht_insert(HTable *symtable, HTItem *new_item) {

    int hash = 0;

    IF_RETURN(!symtable, ERR_INTERNAL)

    hash = createHash(new_item->key);

    /* rewrite value if key exists */
    HTItem *found = ht_search(symtable, new_item->key);
    if (found) {
        found->data_type = new_item->data_type; /* save new data type */
    } /* save new item */
    else {
        if (symtable->first[hash] != NULL) {
            new_item->next = symtable->first[hash];
            symtable->first[hash] = new_item;

        } else {
            symtable->first[hash] = new_item;
        }
    }

    return OK;
}

HTItem *ht_search(HTable *symtable, char *key) {

    IF_RETURN(!symtable, NULL)

    /* hash key */
    int hash = createHash(key);

    HTItem *tmp;
    tmp = symtable->first[hash];

    while(tmp != NULL){
        if (strcmp(tmp->key, key) == 0)
            return tmp;
        tmp = tmp->next; /* move to next item */
    }

    return NULL;

}

int insert_function(HTable *symtable, HTable *function_table, char *key, int params_quantity, tDLList *list) {
   HTItem *item = malloc(sizeof(HTItem));
   IF_RETURN(!item, ERR_INTERNAL)

   item->key = key;
   item->type = FUNCTION;
   item->params_quantity = params_quantity;
   item->symtable = function_table;
   item->list = list;
   item->defined = true;

   return ht_insert(symtable, item);
}

int insert_variable(HTable *symtable, char *key, DATA_TYPE data_type) {
    HTItem *item = malloc(sizeof(HTItem));
    IF_RETURN(!item, ERR_INTERNAL)

    item->key = key;
    item->type = IDENTIFIER;
    item->defined = true;
    item->data_type = data_type;

    return ht_insert(symtable, item);
}
