#include "parser.h"
#include "semantic.h"

//pocet parametrov funkcie na kontrolu
static int countParams = 0;

void htPrintTable( HTable* ptrht ) {
    int maxlen = 0;
    int sumcnt = 0;

    printf ("------------HASH TABLE--------------\n");
    for ( int i=0; i<HTSIZE; i++ ) {
        printf ("%i:",i);
        int cnt = 0;
        HTItem* ptr = ptrht->prvek[i];
        while ( ptr != NULL ) {
            printf (" (%s,%p)",ptr->key,ptrht->prvek[i]->item);
            if ( ptr != NULL )
                cnt++;
            ptr = ptr->next;
        }
        printf ("\n");

        if (cnt > maxlen)
            maxlen = cnt;
        sumcnt+=cnt;
    }

    printf ("------------------------------------\n");
    printf ("Items count %i   The longest list %i\n",sumcnt,maxlen);
    printf ("------------------------------------\n");

}

TOKEN get_next_token() {
    TOKEN token = get_token();

    if (token.type == TERR) {
        exit(ERR_LEX);

    } else {
        return token;
    }
}

tDLList *functionParams(TOKEN token, tDLList *function_param_list) {

    // funkcia ma parametre
    if (token.type == TID) {
        countParams++;

        // vlozenie do zoznamu
        DLInsertLast(function_param_list, &token);
        token = get_next_token();

        while (token.type != Tright_bracket) {

            if (token.type == Tcarka) {
                token = get_next_token();

                if (token.type == TID) {
                    DLInsertLast(function_param_list, &token);
                } else {
                    printf("v error\n");
                    exit(SYNTAX_ERR);
                }

            } else {
                printf("nie je ciarka\n");
                exit(SYNTAX_ERR);
            }
            countParams++;
            token = get_next_token();
        }

    } // funkcia bez parametrov
    else if (token.type == Tright_bracket) {
        DLInsertLast(function_param_list, NULL);
    }
    else {
        printf("error\n");
        exit(SYNTAX_ERR);
    }

    return function_param_list;

}

HTable *recursive_descent_id(int hashtable_switch, TOKEN id, HTable *local_symtable) {

    // zoznam pre hodnoty priradene premennej
    tDLList *list = DLInitList();

    char name[30];
    strcpy(name, id.name);

    TOKEN token;

    if (get_next_token().type == Tassignment) {

        token = get_next_token();
        if (token.type == TEOL) exit(SYNTAX_ERR);

        // kym nie je koniec riadku, ulozenie do zoznamu
        while (token.type != TEOL) {
            // kontrola, ci identifikator alebo identifikator funkcie je definovany
            if (token.type == TID || token.type == TIDFnc) {
                checkIdentifierExists(local_symtable, name, TRUE);

                token = get_next_token();

                // identifikator funkcie
                if (token.type == Tleft_bracket) {
                    token = get_next_token();
                    static int counter = 0;

                    if (token.type == TFLOAT_VALUE || token.type == TSTRING_VALUE || token.type == TINT_VALUE || token.type == TID) {
                        counter++;

                        // kontrola, ci je identifikator definovany
                        if (token.type == TID) {
                            char identifier[30];
                            strcpy(identifier, token.name);

                            checkIdentifierExists(local_symtable, identifier, TRUE);

                        }

                        token = get_next_token();

                        while (token.type != Tright_bracket) {

                            if (token.type != Tcarka) exit(SYNTAX_ERR);

                            token = get_next_token();
                            counter++;

                            if (token.type == TFLOAT_VALUE || token.type == TSTRING_VALUE || token.type == TINT_VALUE || token.type == TID) {
                                // kontrola, ci je identifikator definovany
                                if (token.type == TID) {
                                    char nextId[30];
                                    strcpy(nextId, token.name);

                                    checkIdentifierExists(local_symtable, nextId, TRUE);

                                }

                                token = get_next_token();
                            } else {
                                exit(SYNTAX_ERR);
                            }
                        }
                    } // ziadne parametre sa nepredavaju funkcii
                    else if (token.type == Tright_bracket) {
                        counter = 0;
                    } else {
                        exit(SYNTAX_ERR);
                    }
                    // kontrola na rovnaky pocet parametrov
                    if (counter != countParams) exit(SYNTAX_ERR);
                }
            } else if (token.type != TEOL) {
                // todo strom pre hodnoty
                DLInsertLast(list, &token);
                token = get_next_token();
            }
        }
    } else if (token.type == TEOL) {
        list = NULL;

    } else {
        exit(SYNTAX_ERR);
    }

    // ukladanie do lokalnej tabulky
    if (hashtable_switch) {
        //definicia premennej je mozna aj bez priradenia, kde bude zoznam prazdny
        local_symtable = insert_identifier(name, list, local_symtable);

    } // ukladanie do globalnej tabulky
    else {
        global_hashtable = insert_identifier(name, list, 0);
    }
    htPrintTable(local_symtable);
    return hashtable_switch ? local_symtable : NULL;
}

/**
 *
 * @param local_symtable
 * @param token
 * @return
 */
int recursive_descent_if(int local_symtable, TOKEN token) {


    return 0;
}

/**
 *
 * @param local_symtable
 * @return
 */
int recursive_descent(int hashtable_switch, TOKEN previousToken, HTable *local_symtable) {

    TOKEN token;
    token = local_symtable != NULL ? get_next_token() : previousToken;

    while (1) {

        if (token.type == TEOF) {
            return SYNTAX_OK;

        } else if (token.type == TID) {
            recursive_descent_id(hashtable_switch, token, local_symtable);

        } else if (token.type == Tif) {
            recursive_descent_if(hashtable_switch, token);

        } else {
            printf("zle");
            exit(SYNTAX_ERR);
        }

        token = get_next_token();
    }

}

//pravidlo 25
int function_definition() {

    if (get_next_token().type == Tleft_bracket) {

        // inicializacia zoznamu
        tDLList *function_param_list;
        function_param_list = DLInitList();

        // inicializacia lokalnej tabulky
        HTable *local_symtable = htInit();
        TOKEN token = get_next_token();

        // spracovanie parametrov
        function_param_list = functionParams(token, function_param_list);

        token = get_next_token();

        token.type == TEOL ? recursive_descent(1, token, local_symtable) : exit(SYNTAX_ERR);
    } else {
        exit(SYNTAX_ERR);
    }
    return 0;
}

int recursive_descent_main() {

    previousTokenMain = NULL;
    tmpTokenMain = NULL;

    TOKEN token = get_next_token();

    // koniec suboru
    if (token.type == TEOF) {  // pravidlo 1 -> 2 alebo 5
        if (counter) {
            exit(SYNTAX_ERR);
        }

        return SYNTAX_OK;
    } // definicia funkcie pravidlo 4
    else if (token.type == Tdef) {
        token = get_next_token();

        // ukladanie identifikatoru funkcie do globalnej TS
        // identifikator funkcie nemusi nutne koncit ? || !
        if (token.type == TID || token.type == TIDFnc) {
            // todo kontrola redefinicie funkcie a identifikatora
            //pravidlo 25
            function_definition();
        }
        else {
            exit(SYNTAX_ERR);
        }
    } // scope mimo funkcie
    else {
        recursive_descent(0, token, NULL);
    }

}



// pravidlo 28
int recursive_descent_nextRealParam() {

//    while (token.type != Tright_bracket) {
//
//        if (token.type == Tcarka) {
//            get_next_token();
//
//            if (token.type == TID || token.type == TFLOAT_VALUE || token.type == TSTRING_VALUE) {
//                // todo ulozenie do tabulky
//            } else {
//                exit(SYNTAX_ERR);
//            }
//        } else if (token.type == TID || token.type == TFLOAT_VALUE || token.type == TSTRING_VALUE) {
//            // todo ulozenie do tabulky
//
//        } else {
//            exit(SYNTAX_ERR);
//        }
//
//        get_next_token();
//    }

    return 0;
}

// spracovanie parametrov funkcie
int recursive_descent_realParams() {

//    if (token.type == Tleft_bracket) {
//        get_next_token();
//    }
//
//    // term => identifikator premennej, literal alebo nil
//    if (token.type == TID || token.type == TFLOAT_VALUE || token.type == TSTRING_VALUE) {
//        recursive_descent_nextRealParam();
//    }
//    else {
//        exit(SYNTAX_ERR);
//    }
}

// spracovanie prikazu priradenia
int recursive_descent_rvalue(bool assignment) {

//    static int bracket_count = 0;
//
//    if (token.type == TINT_VALUE || token.type == TSTRING_VALUE || token.type == TFLOAT_VALUE) {
//        // definicia premennej => ulozenie hodnoty do tabulky
//        if (assignment) {
//            // todo ulozenie do tabulky
//        }
//
//        // todo precedencna tabulka
//        while (token.type != TEOL) {
//            get_next_token();
//        }
//
//        return SYNTAX_OK;
//    } // identifikator funkcie, pravidlo 14
//    else if (token.type == TIDFnc) {
//        if (assignment) {
//            // todo ulozenie do tabulky
//        }
//
//        // pravidlo 30, 31 pre lavu zatvorku
//        if (token.type == Tleft_bracket) {
//            bracket_count++;
//        }
//
//        recursive_descent_realParams();
//
//        // pravidlo 32, 33 pre pravu zatvorku
//        if (token.type == Tright_bracket) {
//            bracket_count--;
//            get_next_token();
//        }
//
//        if (token.type != TEOL) {
//            exit(SYNTAX_ERR);
//        }
//
//        // ak nie je rovnaky pocet pravych a lavych zatvoriek => syntax error
//        if (bracket_count != 0) {
//            exit(SYNTAX_ERR);
//        }
//    }

    return 0;
}

int main(int argc, char const *argv[])
{
    // init globalnej tabulky
    list_and_table_init();

    recursive_descent_main();

    return 0;
}