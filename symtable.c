#include "symtable.h"
#include <string.h>

//vytvoreni hashovaciho klice
int zahashuj(tKey key){
  int retval = 1;
  int keylen = strlen(key);
  for( int i=0; i<keylen; i++ ){
    retval = retval + key[i];
  }
  return(retval % HTSIZE);
}


//init HT
HTable * htInit(){

  HTable * hstable = malloc(sizeof(HTable));

  hstable->velikost = HTSIZE;

  hstable->prvek = malloc(sizeof(HTItem *) * HTSIZE );

  for(int i = 0; i < HTSIZE; i++){ //naplni tabulku nully
    hstable->prvek[i] = NULL;
  }

  return hstable;
}


HTItem *htSearch(HTable *ptrht, tKey key){
  if(ptrht == NULL){//ptr neplatny
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


//VKLADANI DO HT
//USPECH == TRUE
//NEUSPECH == FALSE
bool htInsert(HTable *ptrht, tKey key, void *item){
  if(ptrht == NULL){//pokud je ptr neplatny
    fprintf(stderr, "%s\n", "htInsert null pointer");
      return false;
  }

  //podivame se jestli prvek uz v table neni
  //zakomentované protože to kontorlujeme jinde
  /*HTItem *tmp = htSearch(ptrht, key);

  if(tmp != NULL){ //prvek v table je
    fprintf(stderr, "%s\n", "prvek uz je v HT, htInsert");
    return false;
  }*/

  //hashnem si mistecko v policku tabulky
  int hash = zahashuj(key);

  HTItem *new = malloc(sizeof(HTItem)); //alokujem novy
  if(new == NULL){//chyba mallocu
    fprintf(stderr, "%s\n", "chyba mallocu htInsert");
    return false;
  }
  //naplnime novy
  new->item = item;
  new->key = key;
  new->next = ptrht->prvek[hash];

  //vlozime na prvni misto v policku pod hash
  ptrht->prvek[hash] = new;

  //uspech
  return true;

}


//vyhledavani
void *htRead(HTable *ptrht, tKey key){
  if(ptrht == NULL){//pokud je ptr neplatny
    fprintf(stderr, "%s\n", "htRead null pointer");
    return NULL;
  }

  //vyhledame prvek jestli tam je
  HTItem *tmp = htSearch(ptrht, key);
  if(tmp == NULL){
    return NULL;
  }

  return tmp->item;

}
