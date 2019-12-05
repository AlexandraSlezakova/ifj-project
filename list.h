/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file list.h
 * @brief header file for list for arguments of function
 * @author xhladi23 Martin Hladis
 * @author xprasi06 Matej Prasil
 * @author xvadur04 Martin Vadura
 * @author xsleza20 Alexandra Slezakova
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifndef LIST
#define LIST

#define IF_RETURN(CONDITION, VAL) if (CONDITION) return VAL;
#define IF_VALUE_RETURN(VALUE) if (VALUE) return VALUE;


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

#endif