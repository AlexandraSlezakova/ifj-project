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
void *ht_insert(HTable *symtable, HTItem *new_item) {

    int hash = 0;

    IF_RETURN(!symtable, NULL)

    hash = createHash(new_item->key);

    // todo hladat v tabulke, ci tam je ?

    /* end of table*/
    if (symtable->first[hash] != NULL) {
        symtable->first[hash]->next = new_item;
        symtable->first[hash] = new_item;
        new_item->next = NULL;

    } else {
        symtable->first[hash] = new_item;
    }
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

HTItem *insert_function(HTable *symtable, char *key, int params_quantity, bool defined, tDLList *list) {
   HTItem *item = malloc(sizeof(HTItem));
   IF_RETURN(!item, NULL)

   item->key = key;
   item->type = FUNCTION;
   item->defined = defined;
   item->params_quantity = params_quantity;
   item->symtable = symtable;
   item->list = list;

   ht_insert(symtable, item);
   return item;
}

HTItem *insert_variable(HTable *symtable, char *key, DATA_TYPE data_type) {
    HTItem *item = malloc(sizeof(HTItem));
    IF_RETURN(!item, NULL)

    item->key = key;
    item->type = IDENTIFIER;
    item->defined = true;
    item->data_type = data_type;

    ht_insert(symtable, item);
    return item;
}
