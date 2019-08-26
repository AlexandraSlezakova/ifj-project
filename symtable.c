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
   hstable->size = HTSIZE;

    // insert null to table
    for (int i = 0; i < HTSIZE; i++) {
        hstable[i].item = NULL;
    }

    return hstable;
}


/**
 * Create pair = key, value
 */
HTItem *ht_new_item(HTItem *item) {

    HTItem *new_item;

    IF_RETURN((new_item = malloc(sizeof(HTItem))) == NULL, NULL)
    IF_RETURN((new_item->key = strdup(item->key)) == NULL, NULL)

    new_item->symtable = item->symtable;
    new_item->params_quantity = item->params_quantity;
    new_item->defined = item->defined;
    new_item->type = item->type;

    new_item->next = NULL;

    return new_item;
}

/**
 * Insert new item
 */
void *ht_insert(HTable *table, HTItem *new_item) {

    int hash = 0;

    IF_RETURN(!table, NULL)

    hash = createHash(new_item->key);

    // todo hladat v tabulke, ci tam je

    /* end of table*/
    if (table[hash].item) {
        table[hash].item->next = new_item;
        table[hash].item = new_item;
        new_item->next = NULL;

    } else {
        table[hash].item = new_item;
    }
}

HTItem *htSearch(HTable *table, tKey key) {

    IF_RETURN(!table, NULL)

    // hash key
    int hash = createHash(key);

    HTItem *tmp;
    tmp = table[hash].item;

    while(tmp != NULL){
        if (strcmp(tmp->key, key) == 0)
            return tmp;
        tmp = tmp->next; // move to next item
    }

    return NULL;

}

void *htRead(HTable *ptrht, tKey key) {

    IF_RETURN(ptrht == NULL, NULL)

    // search item
    HTItem *tmp = htSearch(ptrht, key);

    return tmp == NULL ? NULL : tmp;
}

HTItem *insert_function(HTable *symtable, tKey key, int params_quantity, bool defined) {
   HTItem *item = malloc(sizeof(HTItem));
   IF_RETURN(!item, NULL)

   item->key = key;
   item->type = FUNCTION;
   item->defined = defined;
   item->params_quantity = params_quantity;
   item->symtable = symtable;

   ht_insert(symtable, item);
   return item;
}

HTItem *insert_variable(HTable *symtable, tKey key) {
    HTItem *item = malloc(sizeof(HTItem));
    IF_RETURN(!item, NULL)

    item->key = key;
    item->type = IDENTIFIER;
    item->defined = true;

    ht_insert(symtable, item);
    return item;
}
