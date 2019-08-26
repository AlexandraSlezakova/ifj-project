#include "parser.h"

void htPrintTable( HTable* ptrht ) {
    int maxlen = 0;
    int sumcnt = 0;

    printf("------------HASH TABLE--------------\n");
    for (int i = 0; i < HTSIZE; i++) {
        printf("%i:", i);
        int cnt = 0;
        HTItem *ptr = ptrht[i].item;
        while (ptr != NULL) {
            printf(" (%s,%p)", ptr->key, ptrht[i].item);
            if (ptr != NULL)
                cnt++;
            ptr = ptr->next;
        }
        printf("\n");

        if (cnt > maxlen)
            maxlen = cnt;
        sumcnt += cnt;
    }
}

int get_next_token() {
    int result = get_token();

    if (result) {
        exit(ERR_LEX);
    }

    return 0;
}


HTable *recursive_descent_id(int hashtable_switch, HTable *local_symtable) {

    // zoznam pre hodnoty priradene premennej
    tDLList *list = DLInitList();

    char *name = copy_name(token.name);

    get_next_token();

    if (token.type == Tassignment) {

        get_next_token();
        if (is_eol(token.type)) return NULL;

        // kym nie je koniec riadku, ulozenie do zoznamu
        while (token.type != TEOL) {
            // kontrola, ci identifikator alebo identifikator funkcie je definovany
            if (is_identifier(token.type)) {

                char *next = copy_name(token.name);

              //  checkIdentifierExists(local_symtable, next, TRUE);

                get_next_token();

                // identifikator funkcie
                if (token.type == Tleft_bracket) {
                    get_next_token();
                    static int counter = 0;

                    if (token.type == TFLOAT_VALUE || token.type == TSTRING_VALUE || token.type == TINT_VALUE || token.type == TID) {
                        counter++;

                        // kontrola, ci je identifikator definovany
                        if (token.type == TID) {
                            char *identifier = malloc(sizeof(char));
                            strcpy(identifier, token.name);

                           // checkIdentifierExists(local_symtable, identifier, TRUE);
                        }

                        get_next_token();

                        while (token.type != Tright_bracket) {

                            if (token.type != Tcarka) exit(SYNTAX_ERR);

                            get_next_token();
                            counter++;

                            if (is_assignment_correct(token.type)) {
                                // kontrola, ci je identifikator definovany
                                if (token.type == TID) {
                                    char *nextId = copy_name(token.name);

                                   // checkIdentifierExists(local_symtable, nextId, TRUE);

                                }

                                get_next_token();
                            } else {
                                return NULL;
                            }
                        }
                    } // ziadne parametre sa nepredavaju funkcii
                    else if (is_right_bracket(token.type)) {
                        counter = 0;
                    } else {
                        exit(SYNTAX_ERR);
                    }
                    // kontrola na rovnaky pocet parametrov
                    //if (counter != countParams) exit(SYNTAX_ERR);
                }
            } else if (token.type != TEOL) {
                // todo strom pre hodnoty
                DLInsertLast(list, &token);
                get_next_token();
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
//        insert_identifier(name, list, local_symtable);

    } // ukladanie do globalnej tabulky
    else {
      //  insert_identifier(name, list, 0);
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
int recursive_descent_if(int local_symtable) {


    return 0;
}

/**
 *
 * @param local_symtable
 * @return
 */
int recursive_descent(int hashtable_switch, HTable *local_symtable)
{

   if (local_symtable != NULL) get_next_token();

    while (1) {
        switch (token.type)
        {
            case TEOF:
                return SYNTAX_ERR;
            case TID:
                recursive_descent_id(hashtable_switch, local_symtable);
                break;
            default:
                return SYNTAX_ERR;
        }

        get_next_token();
    }

}


int function_arguments(HTable *function_symtable, HTItem *new_item) {

    int countParams = 0;
    if (is_left_bracket(token.type)) {
        return SYNTAX_OK;

    /* function has arguments */
    } else if (token.type == TID) {

        // todo redefinicia
        char *name = malloc(sizeof(char));
        strcpy(name, token.name);
//        HTItem *inserted = insert_variable(function_symtable, name);
        //IF_RETURN(!inserted, SYNTAX_ERR)
        new_item->has_params = true;

        /* quantity of params */
        countParams++;

        get_next_token();

        while (token.type != Tright_bracket) {
            IF_RETURN(!is_comma(token.type), SYNTAX_ERR)
            get_next_token();

            IF_RETURN(!(token.type == TID), SYNTAX_ERR)

            strcpy(name, token.name);
//            inserted = insert_variable(function_symtable, name);
  //          IF_RETURN(!inserted, SYNTAX_ERR)

            countParams++;
            get_next_token();
        }

        new_item->params_quantity = countParams;
        return SYNTAX_OK;

    } // function without arguments
    else if (token.type == Tright_bracket) {
        return SYNTAX_OK;
    }

    return SYNTAX_ERR;
}


int recursive_descent_main() {

    get_next_token();
    HTItem *item = NULL;
    HTable *function_table = NULL;

    // end of file
    if (token.type == TEOF) {  // rule 1 -> 2 || 5
        return counter ? SYNTAX_ERR : SYNTAX_OK;

    } // function definition - rule 4
    else if (token.type == Tdef) {
        get_next_token();
        IF_RETURN(!is_identifier(token.type), SYNTAX_ERR)

        char *name = malloc(sizeof(char));
        strcpy(name, token.name);

       HTItem *found = htRead(global_hashtable, name);

       if (found) {
           item = found;
            // todo redefinicia
       } else {
            /* create symtable for function */
           function_table = htInit();
           IF_RETURN(!function_table, ERR_INTERNAL)

            /* add function to global symtable*/
            item = insert_function(function_table, name, 0, true);
        }

        // todo ast
        /* left bracket*/
        get_next_token();
        IF_RETURN(!is_left_bracket(token.type), SYNTAX_ERR)

        get_next_token();
        /* arguments*/
        function_arguments(function_table, item);

    } // global scope
    else {
        //recursive_descent(0, token, NULL);
    }
}

int main(int argc, char const *argv[]) {
    // global table init
    list_and_table_init();

    recursive_descent_main();

    return 0;
}