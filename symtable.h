#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "list.h"
#include "stack.h"


#ifndef HASHTABLE
#define HASHTABLE

//def velikosti tabulky
#define HTSIZE 101

//typ klice
typedef char *tKey;

typedef enum {
    FUNCTION,
    IDENTIFIER,
}HTType;

//tabulka ukazatelu na itemy
//typedef HTItem *HTable[HTSIZE];
typedef struct hashtable{
    struct HTItem *item;
    int size;
} HTable;

/*jednotlive prvky ht*/
typedef struct HTItem{
    tKey key; //klic
    struct HTItem* next;//ukazatel na nasledujici
    void *item; //ukazatel na polozku obsahu /fce nebo promenna/
    HTable *symtable;
    int params_quantity;
    bool defined;
    HTType type;
    bool has_params;
} HTItem;

//hash funkce
int createHash(tKey key);

//init tabulky
HTable * htInit();

//vyhledavani
//vraci null pokud nenajde, jinak vraci ukazatel na ten hledany prvek
HTItem *htSearch(HTable *ptrht, tKey key);

//read
void* htRead(HTable *ptrht, tKey key);

void *ht_insert(HTable *table, HTItem *new_item);
HTItem *insert_function(HTable *symtable, tKey key, int params_quantity, bool defined);
HTItem *insert_variable(HTable *symtable, tKey key);

extern HTable *hashtable;

#endif
