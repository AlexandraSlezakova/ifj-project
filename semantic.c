#include "symtable.h"
#include "symtable.c"
#include "list.h"
#include "list.c"
#include "errors.h"
#include "semantic.h"
#include "scanner.h"
#include <stdio.h>

//tabulka symbolov
HTable *global_hashtable;
tDLList *global_list;
bool success;

/**
 * Inicializacia globalnej tabulky a zoznamu
 */
int list_and_table_init(){

    global_list = DLInitList();

    if (global_list == NULL){
        return ERR_INTERNAL;
    }

    global_hashtable = htInit();

    if ( global_hashtable == NULL){
        return ERR_INTERNAL;
    }

    return SEM_OK;

}

/**
 * Ukladanie identifikatoru do tabulky
 */
int insert_identifier(char *name, struct TToken value, HTable *local_symtable) {
     Tvariable *tvariable = (Tvariable*) malloc(sizeof(struct variable));

     if (tvariable == NULL) {
         return ERR_INTERNAL;
     }

     tvariable->name = name;

     if (value.type == TINT_VALUE) {
         tvariable->value = value.val.ival;

     } else if (value.type == TFLOAT_VALUE) {
         tvariable->value = value.val.fval;

     } else {
         tvariable->value = value.val.p_cval;
     }

     // vkladanie do globalnej tabulky
     if (local_symtable == NULL) {
         return htInsert(global_hashtable, name, tvariable) ? SEM_OK : ERR_INTERNAL;

     } else {
         return htInsert(local_symtable, name, tvariable) ? SEM_OK : ERR_INTERNAL;
     }
}

/**
 * Vkladanie funkcii do tabulky symbolov
 */
int insert_function(tDLList *function_param_list, char *function_name, char ret_type, HTable *local_hashtable){

    Tfunction *in_function = (Tfunction*) malloc(sizeof(struct function));

    if (in_function == NULL){

        return ERR_INTERNAL;
    }

    in_function->name = function_name;
    in_function->params = function_param_list;
    in_function->return_type = ret_type;
    in_function->local_symtable = local_hashtable;

    success = htInsert(global_hashtable, function_name, in_function);

    if (success){

        return SEM_OK;
    }

    success = DLInsertLast(global_list, function_name);
    // vlozenie novej funkcie do zoznamu, aby bolo jednoduchsie vyhladat, ci bola funkcia definovana
    if (success){

        return SEM_OK;
    }

    return ERR_INTERNAL;
}


/**
 * Ukladanie parametrov do lokalnej tabulky a zoznamu parametrov
 */
int insert_function_parameters(tDLList *list, HTable *local_hashtable,
        char *param_name, char *param_data_type){

    Tvariable *parameter = (Tvariable*) malloc(sizeof(struct variable));

    if (parameter == NULL){

        return ERR_INTERNAL;
    }

    parameter->name = param_name;
    parameter->data_type = param_data_type;

    // test, ci je inicializovana lokalna tabulka symbolov
    if (local_hashtable != NULL){

        // vkladanie premennych do lokalnej tabulky symbolov
        success = htInsert(local_hashtable, param_name, parameter);

        // test na uspesnost vkladania parametrov
        if (success){

            return SEM_OK;
        }
    }

    // ukladanie parametrov funkcie do zoznamu
    success =  DLInsertLast(list, parameter);

    if (success){

        return  SEM_OK;
    }

    return ERR_INTERNAL;
}

/**
 * Prida vstavanu funkciu do tabulky
 */
int insert_built_in_function(tDLList *function_param, char *function_name, char ret_type){

    Tfunction *in_function = (Tfunction*) malloc(sizeof(struct function));

    if (in_function == NULL){

        return ERR_INTERNAL;
    }

    in_function->name = function_name;
    in_function->params = function_param;
    in_function->return_type = ret_type;
    in_function->local_symtable = NULL;

    success = htInsert(global_hashtable, function_name, in_function);

    if (success){

        return SEM_OK;
    }

    return ERR_INTERNAL;
}

/**
 * Ukladanie vstavanych funkcii do tabulky symbolov a ukladanie parametrov do zoznamu
 */
int save_built_in_function(){
//
//    tDLList *parameters;
//
//    // Vkladanie funkcie "inputs" do tabulky
//    if ((parameters = DLInitList()) == NULL){
//        return ERR_INTERNAL;
//    }
//    if (insert_function_parameters(parameters, NULL, NULL, ' ') != SEM_OK) { // to do!!!!
//        return SEM_ERR_COMPAT;
//    }
//    if (insert_built_in_function(parameters, "inputs", 's') != SEM_OK){
//        return ERR_INTERNAL;
//    }
//
//
//
//    // Vkladanie funkcie "inputi" do tabulky
//    if ((parameters = DLInitList()) == NULL){
//        return ERR_INTERNAL;
//    }
//    if (insert_function_parameters(parameters, NULL, NULL, ' ') != SEM_OK){ // to do!!!!
//        return SEM_ERR_COMPAT;
//    }
//    if (insert_built_in_function(parameters, "inputi", 'i') != SEM_OK){
//        return ERR_INTERNAL;
//    }
//
//
//    // Vkladanie funkcie "inputf" do tabulky
//    if ((parameters = DLInitList()) == NULL){
//        return ERR_INTERNAL;
//    }
//    if (insert_function_parameters(parameters, NULL, NULL, ' ') != SEM_OK){ // to do!!!!
//        return SEM_ERR_COMPAT;
//    }
//    if (insert_built_in_function(parameters, "inputf", 'f') != SEM_OK){
//        return ERR_INTERNAL;
//    }
//
//    /* Vkladanie funkcie "print" do tabulky
//     * ?
//     * ?
//     * ?
//     * ?
//     * */
//    if ((parameters = DLInitList()) == NULL){
//        return ERR_INTERNAL;
//    }
//    if (insert_function_parameters(parameters, NULL, "s", 's') != SEM_OK){
//            return ERR_INTERNAL;
//        }
//    if (insert_built_in_function(parameters, "print", 's') != SEM_OK){
//        return ERR_INTERNAL;
//    }
//
//
//
//    // Vkladanie funkcie "length" do tabulky
//    if ((parameters = DLInitList()) == NULL){
//        return ERR_INTERNAL;
//    }
//    if (insert_function_parameters(parameters, NULL, "s", 's') != SEM_OK){
//        return ERR_INTERNAL;
//    }
//    if (insert_built_in_function(parameters, "length", 'i') != SEM_OK){
//        return ERR_INTERNAL;
//    }
//
//
//
//    // Vkladanie funkcie "substr" do tabulky
//    if ((parameters = DLInitList()) == NULL){
//        return ERR_INTERNAL;
//    }
//    if (insert_function_parameters(parameters, NULL, "s", 's') != SEM_OK ||
//        insert_function_parameters(parameters, NULL, "i", 'i') != SEM_OK ||
//        insert_function_parameters(parameters, NULL, "n", 'i') != SEM_OK){
//        return ERR_INTERNAL;
//    }
//    if (insert_built_in_function(parameters, "substr", 's') != 0){
//        return ERR_INTERNAL;
//    }
//
//
//
//    // Vkladanie funkcie "ord" do tabulky
//    if ((parameters = DLInitList()) == NULL){
//        return ERR_INTERNAL;
//    }
//    if (insert_function_parameters(parameters, NULL, "s", 's') != SEM_OK ||
//        insert_function_parameters(parameters, NULL, "i", 'i') != SEM_OK ){
//        return ERR_INTERNAL;
//    }
//    if (insert_built_in_function(parameters, "ord", 'i') != 0){
//        return ERR_INTERNAL;
//    }
//
//
//
//    // Vkladanie funkcie "chr" do tabulky
//    if ((parameters = DLInitList()) == NULL){
//        return ERR_INTERNAL;
//    }
//    if (insert_function_parameters(parameters, NULL, "i", 'i') != SEM_OK ){
//        return ERR_INTERNAL;
//    }
//    if (insert_built_in_function(parameters, "chr", 'i') != SEM_OK){
//        return ERR_INTERNAL;
//    }
//
//    return SEM_OK;

}

/**
 * Kontroluje, ci funkcia existuje
 */
int checkFunctionExists(char* function_name,char typ, tDLList * local_list, HTable * local_hashtable ){

    success = DLSearch(global_list,function_name);

    if(success){
        //funkce uz v tabulce je takze ji zkousime deklarovat podruhe ->chyba
        return SEM_ERR_UNDEF_VAR;
    }

    //pokud funkce v tabulce neni, vlozime ji tam


    int error;
    //error = insert_function(local_list,function_name, typ, local_hashtable );
    //todo
    return 0;
}

/**
 *  Kontroluje existenciu premennej
 */
bool checkVariableExists(HTable * local_hashtable, char * var_name, char *typ ){


    Tvariable * varT = htRead(local_hashtable, var_name);
    if(varT != NULL ){
        //promenna uz v tabulce je, zmenime jeji typ
        varT->data_type = typ;

        return true;
    }

    return false;

}

/**
 * Vytvorenie novej premennej a vlozenie do lokalnej tabulky symbolov
 */
int createVariable(HTable * local_hashtable, char * var_name, char *typ){
    //todo
    //return insert_function_parameters(NULL, local_hashtable, var_name, typ );
    return 0;
}

void checkTypesCorrect(char *var_name1, char var_type1, char *var_name2,char var_type2){


}

