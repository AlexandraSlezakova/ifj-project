/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file list.c
 * @brief list for arguments of function
 * @author
 * @author
 * @author
 * @author
 */

#include "list.h"

tDLList * DLInitList () {

    tDLList *list = malloc(sizeof(tDLList));

    list->First = NULL;
    list->Last = NULL;

    return list;
}

bool DLInsertLast(tDLList *L,  char *ptr) {

    /* new item of list */
    struct tDLElem *item = malloc(sizeof(struct tDLElem));

    IF_RETURN(!item, false)

    item->data = ptr;
    item->rptr = NULL;
    item->lptr = L->Last;

    if(L->First != NULL){
        L->Last->rptr = item;
    } else{
        L->First = item;
    }

    L->Last = item;

    return true;
}

//bool DLInsertFirst (tDLList *L, char *ptr) {
//    struct tDLElem *item = malloc(sizeof(struct tDLElem));
//
//    if( item == NULL ){ /* osetreni spravneho mallocu */
//        return FALSE;
//    }
//
//    item->data = ptr;   /*prida mu hodnotu*/
//
//    item->lptr = NULL;  /* levy pt prvniho null */
//    item->rptr = L->First; /* pravy p na minuly prvni/null */
//
//    if(L->First == NULL){ /* pakud je prvni */
//        L->Last = item; /* je i posledni */
//    }else{
//        L->First->lptr = item; /* minuly prvni ukazuje zpet */
//    }
//
//    L->First = item; /* novy prvni */
//
//    return TRUE;
//}

//bool DLDeleteFirst (tDLList *L) {
///**
//** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita
//** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
//**/
//
//
//
//    if(L->First == NULL){ /* osetreni prazdnoty*/
//        return FALSE;
//    }
//
//
//
//    if(L->First->rptr == NULL){ /* pokud je tam jen jeden */
//        free(L->First); /* uvolniho*/
//        L->First = NULL; /* zresetuje ukazatele */
//        L->Last = NULL;
//        return TRUE;
//    }else{
//        L->First = L->First->rptr; /* nastavim prvni druhy */
//        free(L->First->lptr); /* uvolni prvni */
//        L->First->lptr = NULL; /* nuluje ukazatel druheho na prvni */
//    }
//
//    return TRUE;
//
//}
//
//
//
//
//
//bool DLDeleteLast (tDLList *L) {
///**
//** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
//** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
//**/
//
//    if(L->First == NULL){ /* osetreni prazdnoty*/
//        return FALSE;
//    }
//
//
//
//    if(L->First == L->Last ){ /* pokud je tam jen jeden */
//        free(L->First); /* uvolniho*/
//        L->First = NULL; /* zresetuje ukazatele */
//        L->Last = NULL;
//        return TRUE;
//    }else{
//        L->Last = L->Last->lptr; /* poslednim predposledni */
//        free(L->Last->rptr); /* smazani posledniho */
//        L->Last->rptr = NULL; /* null konce */
//    }
//
//    return TRUE;
//}
//
//
//
////VYHLEDANI V SEZNAMU
//bool DLSearch(tDLList * listPtr, char* key){
//
//    if(listPtr == NULL){
//        return FALSE;
//    }
//
//    tDLElemPtr tmp = NULL;
//    tmp = listPtr->First;
//
//    while(tmp != NULL){
//
//        if(tmp->data == key){
//            return  TRUE;
//        }
//        tmp = tmp->rptr;
//
//    }
//
//    return FALSE;
//}
