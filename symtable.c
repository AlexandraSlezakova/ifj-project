#include "symtable.h"
#include <string.h>

//vytvoreni hashovaciho klice
int zahashuj(tKey key){

    int retval = 1;
    int keylen = strlen(key);

    for(int i = 0; i < keylen; i++) {
        retval = retval + key[i];
    }

    return(retval % HTSIZE);
}


//init HT
HTable *htInit(){

    HTable *hstable = NULL;

    if ((hstable = malloc(sizeof(HTable)))== NULL) {
      return NULL;
    }

    hstable->velikost = HTSIZE;

    if ((hstable->prvek = malloc(sizeof(HTItem *) * HTSIZE )) == NULL) {
        return NULL;
    }

    for (int i = 0; i < HTSIZE; i++) { //naplni tabulku nully
        hstable->prvek[i] = NULL;
    }

    return hstable;
}


/**
 * Vytvorenie paru/prvku tabulky = kluc, hodnota
 */
HTItem *ht_newpair( char *key, char *value ) {
    HTItem *newpair;

    if ((newpair = malloc(sizeof(HTItem))) == NULL) {
        return NULL;
    }

    if ((newpair->key = strdup(key)) == NULL) {
        return NULL;
    }

    if ((newpair->item = strdup(value)) == NULL) {
        return NULL;
    }

    newpair->next = NULL;

    return newpair;
}

/**
 * Vkladanie do tabulky
 */
HTable *htInsertNew(HTable *ptrht, tKey key, void *item) {

    int hash = 0;
    HTItem *next = NULL;
    HTItem *last = NULL;
    HTItem *newpair = NULL;

    //pokud je ptr neplatny
    if (ptrht == NULL) {
        fprintf(stderr, "%s\n", "htInsert null pointer");
        return false;
    }

    //hashnem si mistecko v policku tabulky
    hash = zahashuj(key);

    next = ptrht->prvek[hash];

    while (next != NULL && next->key != NULL && strcmp(key, next->key) > 0) {
        last = next;
        next = next->next;
    }

    // test, ci sa uz v tabulke nenachadza prvok s rovnakym klucom
    if(next != NULL && next->key != NULL && strcmp(key, next->key) == 0 ) {
        free(next->item);
        // vytvorenie kopie
        next->item = strdup(item);

    } // pridanie noveho prvku do tabulky
    else {
        // vytvorenie paru
        newpair = ht_newpair( key, item );

        // ak je to prvy prvok, ktory vkladame
        if (next == ptrht->prvek[hash]) {
            newpair->next = next;
            ptrht->prvek[hash] = newpair;

        } // koniec tabulky
        else if (next == NULL) {
            last->next = newpair;

        } // stred tabulky
        else  {
            newpair->next = next;
            last->next = newpair;
        }
    }

    return ptrht;
}

HTItem *htSearch(HTable *ptrht, tKey key) {

    if(ptrht == NULL) {//ptr neplatny
        fprintf(stderr, "%s\n", "htsearch null pointer");
        return NULL;
    }

    //zjistime hash
    int hash = zahashuj(key);

    //do tmp dame hodnotu prvniho prvku seznamu na miste kde vychazi hash
    HTItem *tmp = ptrht->prvek[hash];

    //loop dokud nenajdeme nebo nejsme v nullu
    while( tmp != NULL && tmp->next->key != key ){
        tmp = tmp->next;//posunem se na dalsi
    }

    return tmp;

}

//vyhledavani
void *htRead(HTable *ptrht, tKey key) {

  if(ptrht == NULL){//pokud je ptr neplatny
    fprintf(stderr, "%s\n", "htRead null pointer");
    return NULL;
  }

  //vyhledame prvek jestli tam je
  HTItem *tmp = htSearch(ptrht, key);

  if(tmp == NULL){
    return NULL;
  }

  return tmp;

}
