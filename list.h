
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef LIST
#define LIST

#define FALSE 0
#define TRUE 1

extern int errflg;
extern int solved;

/**
 *Struktura pro prvek v listu
 */
typedef struct tDLElem {                 /* prvek dvousměrně vázaného seznamu */
    char* data;                                            /* užitečná data */
    struct tDLElem *lptr;          /* ukazatel na předchozí prvek seznamu */
    struct tDLElem *rptr;        /* ukazatel na následující prvek seznamu */
} *tDLElemPtr;

/**
 * Struktura listu
 */
typedef struct {                                  /* dvousměrně vázaný seznam */
    tDLElemPtr First;                      /* ukazatel na první prvek seznamu */
    tDLElemPtr Last;                    /* ukazatel na posledni prvek seznamu */
} tDLList;

/**
 * Inicializuje list
 * @return ukazatel na inicializovaný list
 */
tDLList * DLInitList ();
/**
 *
 * @param tDLList * ukazatel na list kam chceme přidat prvek
 * @param void *  ukazatel na strukturu variable/function kterou chceme přiřadit
 * @return True pokud uspech, jinak FALSE
 */
bool DLInsertFirst (tDLList *,  char *);
/**
 * @param tDLList * ukazatel na list kam chceme přidat prvek
 * @param void * ukazatel na strukturu variable/function kterou chceme přiřadit
 * @return True pokud uspech, jinak FALSE
 */
bool DLInsertLast(tDLList *,  char *);
/**
 * @param tDLList * ukazatel na list kde chceme mazat
 * @return True pokud uspech, jinak FALSE
 */
bool DLDeleteLast (tDLList *);
/**
 * @param tDLList * ukazatel na list kde chceme mazat
 * @return True pokud uspech, jinak FALSE
 */
bool DLDeleteFirst(tDLList *);
/**
 * @param tDLList * ukazatel na list kde chceme vyhledavat
 * @param char* string s nazvem
 * @return True pokud uspech, jinak FALSE
 */
bool DLSearch(tDLList *, char*);


#endif