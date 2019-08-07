#include "scanner.c"
#include "parser.h"
#include "errors.h"
#include "semantic.c"
#include "scanner.h"

int get_next_token() {
    int result = get_token();

    if (result) {
        exit(ERR_LEX);
    }

    return 0;
}

HTable *recursive_descent_id(int local_symtable) {

    HTable *local_symtable_init;
    // inicializacia tabulky
    local_symtable_init =  local_symtable ? htInit() : NULL;

    previousToken = malloc(sizeof(struct TToken));
    *previousToken = token;
    get_next_token();

    if (token.type == Tassignment) {

        get_next_token();

        if (token.type == TINT_VALUE || token.type == TFLOAT_VALUE || token.type == TSTRING_VALUE) {
            // ukladanie do tabulky
            insert_identifier(previousToken->val.p_cval, token, local_symtable_init);
        }

    }


    // todo ukladanie identifikatoru do tabulky

    // ak je dalsi token priradenie, ziskame zase dalsi token
    if (token.type == Tassignment) {
        get_next_token();
        recursive_descent_rvalue(true);
    }

    return 0;
}

int recursive_descent(int local_symtable) {
    previousToken = NULL;
    tmpToken = NULL;

    while(1) {
        get_next_token();

        if (token.type == TID) {
            recursive_descent_id(local_symtable);
        }
    }
}

HTable *recursive_descent_in_function() {
    HTable *local_symtable;
    local_symtable = htInit();


    return local_symtable;
}

//pravidlo 25
int function_definition(struct TToken old_token) {
    struct TToken *previous_token;
    previous_token = malloc(sizeof(struct TToken));

    *previous_token = old_token;

    get_next_token();
    //printf("pred %d", token.type);
    if (token.type == Tleft_bracket) {
        get_next_token();

        // inicializacia zoznamu
        tDLList *function_param_list;
        function_param_list = DLInitList();

        // funkcia ma parametre
        if (token.type == TID) {

            // vlozenie do zoznamu
            DLInsertLast(function_param_list, &token);
            get_next_token();

            // spracovanie viacerych parametrov
            while (token.type != Tright_bracket) {
                if (token.type == Tcarka) {
                    get_next_token();

                    if (token.type == TID) {
                        DLInsertLast(function_param_list, &token);
                        printf("vo if %d\n", token.type);
                    } else {
                        printf("v error\n");
                        exit(SYNTAX_ERR);
                    }

                } else {
                    printf("nie je ciarka\n");
                    exit(SYNTAX_ERR);
                }
                get_next_token();
            }


        } // funkcia bez parametrov
        else if (token.type == Tright_bracket) {
            DLInsertLast(function_param_list, NULL);
        }
        else {
            printf("error\n");
            exit(SYNTAX_ERR);
        }

        get_next_token();
        token.type == TEOL ? recursive_descent_in_function() : exit(SYNTAX_ERR);
    } else {
        exit(SYNTAX_ERR);
    }
    return 0;
}

int recursive_descent_main() {

    previousTokenMain = NULL;
    tmpTokenMain = NULL;

    while (1) {
        get_next_token();

        // koniec suboru
        if (token.type == TEOF) {  // pravidlo 1 -> 2 alebo 5
            if (counter) {
                exit(SYNTAX_ERR);
            }

            return SYNTAX_OK;
        } // definicia funkcie pravidlo 4
        else if (token.type == Tdef) {
            get_next_token();

            // ukladanie identifikatoru funkcie do globalnej TS
            // identifikator funkcie nemusi nutne koncit ? || !
            if (token.type == TID || token.type == TIDFnc) {
                //pravidlo 25
                function_definition(token);
            }
            else {
                exit(SYNTAX_ERR);
            }
        }
    }
}



// pravidlo 28
int recursive_descent_nextRealParam() {

    while (token.type != Tright_bracket) {

        if (token.type == Tcarka) {
            get_next_token();

            if (token.type == TID || token.type == TFLOAT_VALUE || token.type == TSTRING_VALUE) {
                // todo ulozenie do tabulky
            } else {
                exit(SYNTAX_ERR);
            }
        } else if (token.type == TID || token.type == TFLOAT_VALUE || token.type == TSTRING_VALUE) {
            // todo ulozenie do tabulky

        } else {
            exit(SYNTAX_ERR);
        }

        get_next_token();
    }

    return 0;
}

// spracovanie parametrov funkcie
int recursive_descent_realParams() {

    if (token.type == Tleft_bracket) {
        get_next_token();
    }

    // term => identifikator premennej, literal alebo nil
    if (token.type == TID || token.type == TFLOAT_VALUE || token.type == TSTRING_VALUE) {
        recursive_descent_nextRealParam();
    }
    else {
        exit(SYNTAX_ERR);
    }
}

// spracovanie prikazu priradenia
int recursive_descent_rvalue(bool assignment) {

    static int bracket_count = 0;

    if (token.type == TINT_VALUE || token.type == TSTRING_VALUE || token.type == TFLOAT_VALUE) {
      // definicia premennej => ulozenie hodnoty do tabulky
      if (assignment) {
          // todo ulozenie do tabulky
      }

      // todo precedencna tabulka
      while (token.type != TEOL) {
          get_next_token();
      }

      return SYNTAX_OK;
    } // identifikator funkcie, pravidlo 14
    else if (token.type == TIDFnc) {
      if (assignment) {
          // todo ulozenie do tabulky
      }

      // pravidlo 30, 31 pre lavu zatvorku
      if (token.type == Tleft_bracket) {
          bracket_count++;
      }

      recursive_descent_realParams();

      // pravidlo 32, 33 pre pravu zatvorku
      if (token.type == Tright_bracket) {
          bracket_count--;
          get_next_token();
      }

      if (token.type != TEOL) {
          exit(SYNTAX_ERR);
      }

      // ak nie je rovnaky pocet pravych a lavych zatvoriek => syntax error
      if (bracket_count != 0) {
          exit(SYNTAX_ERR);
      }
    }

    return 0;
}

int main(int argc, char const *argv[])
{
    // init globalnej tabulky
    list_and_table_init();

    recursive_descent_main();
    return 0;
}