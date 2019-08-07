/**
 *
 * @file list.c
 * @authors Tomáš Majerech, Alexandra Slezáková
 * @date 25.11.2018
 * @brief Soubor pro práci s listem
 *
 */

#include "list.h"




int errflg;
int solved;

tDLList * DLInitList () {
/**
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/

    tDLList *L = malloc(sizeof(tDLList));


    L->First = NULL;
    L->Last = NULL;


    return L;
}

bool DLInsertFirst (tDLList *L, void *ptr) {
/**
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

    struct tDLElem *ptrNaNovy = malloc(sizeof(struct tDLElem)); /*vytvori novy prvek*/

    if( ptrNaNovy == NULL ){ /* osetreni spravneho mallocu */
        return FALSE;
    }

    ptrNaNovy->data = ptr;   /*prida mu hodnotu*/

    ptrNaNovy->lptr = NULL;  /* levy pt prvniho null */
    ptrNaNovy->rptr = L->First; /* pravy p na minuly prvni/null */

    if(L->First == NULL){ /* pakud je prvni */
        L->Last = ptrNaNovy; /* je i posledni */
    }else{
        L->First->lptr = ptrNaNovy; /* minuly prvni ukazuje zpet */
    }

    L->First = ptrNaNovy; /* novy prvni */

    return TRUE;
}





bool DLInsertLast(tDLList *L,  void *ptr) {
/**
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

    struct tDLElem *ptrNaNovy = malloc(sizeof(struct tDLElem)); /*vytvori novy prvek*/

    if( ptrNaNovy == NULL ){ /* osetreni spravneho mallocu */
        return FALSE;
    }

    ptrNaNovy->data = ptr; /* naplni hodnotou */


    ptrNaNovy->rptr = NULL; /* posledni doprava je null */
    ptrNaNovy->lptr = L->Last; /* lpt na minuly posledni */

    if(L->First != NULL){
        L->Last->rptr = ptrNaNovy; /* puvodni psoledni ukazuje doprava pokud existuje */
    } else{
        L->First = ptrNaNovy;  /* prvni je zaroven posoedni */
    }

    L->Last = ptrNaNovy; /* novy je psoledni */

    return TRUE;
}







bool DLDeleteFirst (tDLList *L) {
/**
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/



    if(L->First == NULL){ /* osetreni prazdnoty*/
        return FALSE;
    }



    if(L->First->rptr == NULL){ /* pokud je tam jen jeden */
        free(L->First); /* uvolniho*/
        L->First = NULL; /* zresetuje ukazatele */
        L->Last = NULL;
        return TRUE;
    }else{
        L->First = L->First->rptr; /* nastavim prvni druhy */
        free(L->First->lptr); /* uvolni prvni */
        L->First->lptr = NULL; /* nuluje ukazatel druheho na prvni */
    }

    return TRUE;

}





bool DLDeleteLast (tDLList *L) {
/**
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/

    if(L->First == NULL){ /* osetreni prazdnoty*/
        return FALSE;
    }



    if(L->First == L->Last ){ /* pokud je tam jen jeden */
        free(L->First); /* uvolniho*/
        L->First = NULL; /* zresetuje ukazatele */
        L->Last = NULL;
        return TRUE;
    }else{
        L->Last = L->Last->lptr; /* poslednim predposledni */
        free(L->Last->rptr); /* smazani posledniho */
        L->Last->rptr = NULL; /* null konce */
    }

    return TRUE;
}



//VYHLEDANI V SEZNAMU
bool DLSearch(tDLList * listPtr, char* key){
    /**
     *
     *
     */

    if(listPtr == NULL){
        return FALSE;
    }

    tDLElemPtr tmp = NULL;
    tmp = listPtr->First;

    while(tmp != NULL){

        if(tmp->data == key){
            return  TRUE;
        }
        tmp = tmp->rptr;

    }

    return FALSE;
}
