/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file errors.h
 * @brief header file for error codes
 * @author
 * @author
 * @author
 * @author
 */

#ifndef IFJ_ERRORS_H
#define IFJ_ERRORS_H

#define OK                          0  // vsetko ok
#define TOKEN_OK			        0  // token je ok
#define TOKEN_ERR                   1  // token error
#define SYNTAX_OK 					0  // Syntaxe je ok
#define SEM_OK                      0  // Semantika je ok
#define ERR_LEX		            	1  // Chyba v programu lexikální analýzy
#define SYNTAX_ERR					2  // Chyba v programu v rámci syntaktické analýzy
#define SEM_ERR_UNDEF_VAR		    3  // Sémantická chyba v programu - nedefinovaná funkce / proměnná, pokus o redefinici funkce nebo proměnné
#define SEM_ERR_COMPAT			    4  // Sémantická/běhová chyba typové kompatibilty v aritmetických, řetězových a relačních výrazech
#define SEM_ERR_PARAM_NUM           5  // Sémantická chyba v programu, špatný počet parametrů u volání funkce
#define SEM_ERR_OTHER				6  // ostatní sémantické chyby
#define ERR_ZERO_DIV				9  // běhová chyba dělení nulou
#define ERR_INTERNAL				99 // interní chyba překladače, chyba alokace paměti, atd.

#endif