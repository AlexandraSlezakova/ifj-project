/**
 * @file semantic.h
 * @authors Alexandra Slezakova, Tomas Majerech
 * @date 25.11.2018
 * @brief Subor obsahujuci hlavicky pomocnyc funkcii semantickej analyzy
 */
#include "symtable.h"
#ifndef _SEMANTIC_H
#define _SEMANTIC_H


/**
 *  Inicializuje zoznam a tabulku symbolov
 *  @return SEM_OK, ERR_INTERNAL
 */
int list_and_table_init();

HTable *init_local_table();

/**
 *  Vkladanie funkcii
 *  @param function_param_list parametre funkcie zoradene do zoznamu
 *  @param function_name nazov funkcie
 *  @param ret_type navratovy typ funkcie
 *  @param defined true, ak funkcia bola definovana
 *  @param local_hashtable tabulka symbolov pre lokalne premenne
 *  @return SEM_OK, ERR_INTERNAL
 */
int insert_function(tDLList *function_param_list, char *function_name, char ret_type, HTable *local_hashtable);

/**
 * Ukladanie parametrov do lokalnej tabulky a zoznamu parametrov
 * @param list zoznam parametrov
 * @param local_hashtable tabulka symbolov pre lokalne premenne
 * @param param_name nazov premennej
 * @param param_data_type datovy typ premennej
 * @return SEM_OK, ERR_INTERNAL
 */
int insert_function_parameters(tDLList *list, HTable *local_hashtable, char *param_name, char *param_data_type);

/**
 * Pridanie vstavanej funkcie do tabulky symbbolov
 * @param function_param zoznam parametrov funkcie
 * @param function_name nazov funkcie
 * @param ret_type navratovy typ funkcie
 * @return SEM_OK, ERR_INTERNAL
 */
int insert_built_in_function(tDLList *function_param, char *function_name, char ret_type);

/**
 * Ukladanie vstavanych funkcii do tabulky symbolov a ukladanie parametrov do zoznamu
 * @return SEM_OK, ERR_INTERNAL
 */
int save_built_in_function();

/**
 * PŘED ZAVOLÁNÍM FUNKCE JE POTŘEBA VYTVOŘIT
 * A NAPLNIT LIST S PARAMETRY FUNKCE FUNKCÍ INSERT FUNCTION PARAMETERS
 * a předat localní tabulku symbolů
 *
 * @param function_name
 * @param typ
 * @param local_list
 * @param local_hashtable
 * @return SEM_ERR_UNDEF_VAR
 */
int checkFunctionExists(char* function_name,char typ, tDLList * local_list, HTable * local_hashtable);

/**
 * Kontroluje existenciu premennej
 * Ak premenna existuje, aktualizuje jej datovy typ
 * @param local_hashtable lokalna tabulka premennych
 * @param var_name nazov premennej
 * @param typ typ premennej
 * @return
 */
bool checkVariableExists(HTable * local_hashtable, char * var_name, char *typ );

/**
 * Vytvorenie novej premennej a vlozenie do lokalnej tabulky symbolov
 * @param local_hashtable lokalna tabulka symbolov
 * @param var_name nazov premennej
 * @param typ typ premennej
 * @return SEM_OK, ERR_INTERNAL
 */
int createVariable(HTable * local_hashtable, char * var_name, char *typ);



#endif