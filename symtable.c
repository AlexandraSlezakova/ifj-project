#include "symtable.h"
#include <string.h>

/**
 * Hash function
 */
int createHash(tKey key){

    int retval = 1;
    int keylen = strlen(key);

    for(int i = 0; i < keylen; i++) {
        retval = retval + key[i];
    }

    return(keylen % HTSIZE);
}


/**
 * Init table
 */
HTable *htInit(){

    HTable *hstable = NULL;

   hstable = malloc(sizeof(HTable));
   IF_RETURN(!hstable, NULL)

    // insert null to table
    for (int i = 0; i < HTSIZE; i++) {
        (*hstable).first[i] = NULL;
    }

    return hstable;
}


/**
 * Insert new item
 */
void *ht_insert(HTable *table, HTItem *new_item) {

    int hash = 0;

    IF_RETURN(!table, NULL)

    hash = createHash(new_item->key);

    // todo hladat v tabulke, ci tam je ?

    /* end of table*/
    if (table->first[hash] != NULL) {
        table->first[hash]->next = new_item;
        table->first[hash] = new_item;
        new_item->next = NULL;

    } else {
        table->first[hash] = new_item;
    }
}

HTItem *htSearch(HTable *table, tKey key) {

    IF_RETURN(!table, NULL)

    /* hash key */
    int hash = createHash(key);

    HTItem *tmp;
    tmp = table->first[hash];

    while(tmp != NULL){
        if (strcmp(tmp->key, key) == 0)
            return tmp;
        tmp = tmp->next; /* move to next item */
    }

    return NULL;

}

HTItem *insert_function(HTable *symtable, tKey key, int params_quantity, bool defined, tDLList *list) {
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

HTItem *insert_variable(HTable *symtable, tKey key, DATA_TYPE data_type) {
    HTItem *item = malloc(sizeof(HTItem));
    IF_RETURN(!item, NULL)

    item->key = key;
    item->type = IDENTIFIER;
    item->defined = true;
    item->data_type = data_type;

    ht_insert(symtable, item);
    return item;
}
