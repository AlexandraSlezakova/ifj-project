



#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "list.h"


#ifndef HASHTABLE
#define HASHTABLE





//def velikosti tabulky
#define HTSIZE 101

//typ klice
typedef char *tKey;

/*jednotlive prvky ht*/
typedef struct HTItem{
  tKey key; //klic
  struct HTItem* next;//ukazatel na nasledujici
  void *item; //ukazatel na polozku obsahu /fce nebo promenna/
} HTItem;

//tabulka ukazatelu na itemy
//typedef HTItem *HTable[HTSIZE];

typedef struct hashtable{
    int velikost;
    struct HTItem **prvek;
} HTable;


//hash funkce
int zahashuj(tKey key);

//init tabulky
HTable * htInit();

//vyhledavani
//vraci null pokud nenajde, jinak vraci ukazatel na ten hledany prvek
HTItem *htSearch(HTable *ptrht, tKey key);

//VKLADANI DO HT
//USPECH == TRUE
//NEUSPECH == FALSE
bool htInsert(HTable *ptrht, tKey key, void *item);

//read
void* htRead(HTable *ptrht, tKey key);


//struktura pro promennou
typedef struct variable
{
    char *name;     //jmeno
    void *value;	//hodnota
} Tvariable;


//struktura pro fuknci
typedef struct function
{
    char *name; //jmeno funkce
    tDLList *params; //list parametru
    HTable *local_symtable; //tabulka lokalnich promennych
    char return_type; //navratovy typ funkce

} Tfunction;



extern HTable *hashtable;


#endif
