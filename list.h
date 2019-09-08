/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file list.h
 * @brief header file for list for arguments of function
 * @author
 * @author
 * @author
 * @author
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef LIST
#define LIST

#define IF_RETURN(CONDITION, VAL) if (CONDITION) return VAL;


typedef struct tDLElem {
    char* data;
    struct tDLElem *lptr;
    struct tDLElem *rptr;
} *tDLElemPtr;


typedef struct {
    tDLElemPtr First;
    tDLElemPtr Last;
} tDLList;

/**
 * Init of list
 * @return pointer to list
 */
tDLList * DLInitList ();

/**
 * Insert item of list as last item
 * @param L list
 * @param ptr item
 * @return true if success
 */
bool DLInsertLast(tDLList *L,  char *ptr);

//bool DLInsertFirst (tDLList *,  char *);

/**
 * @param tDLList * ukazatel na list kde chceme mazat
 * @return True pokud uspech, jinak FALSE
 */
//bool DLDeleteLast (tDLList *);
/**
 * @param tDLList * ukazatel na list kde chceme mazat
 * @return True pokud uspech, jinak FALSE
 */
//bool DLDeleteFirst(tDLList *);
/**
 * @param tDLList * ukazatel na list kde chceme vyhledavat
 * @param char* string s nazvem
 * @return True pokud uspech, jinak FALSE
 */
//bool DLSearch(tDLList *, char*);


#endif