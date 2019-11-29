/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file errors.h
 * @brief header file for error codes
 * @author xhladi23 Martin Hladis
 * @author xprasi06 Matej Prasil
 * @author xvadur04 Martin Vadura
 * @author xsleza20 Alexandra Slezakova
 */

#ifndef IFJ_ERRORS_H
#define IFJ_ERRORS_H

#define OK                          0  /* no error */
#define TOKEN_OK			        0  /* no token error */
#define TOKEN_ERR                   1  /* token error */
#define SYNTAX_OK 					0  /* no syntax error */
#define SEM_OK                      0  /* no semantic error */
#define LEX_ERR		            	1  /* lexical analysis error */
#define SYNTAX_ERR					2  /* syntax error */
#define SEM_ERR_UNDEF_VAR		    3  /* semantic error - undefined function / variable, redefinition of function or variable */
#define SEM_ERR_COMPAT			    4  /* semantic error - wrong data types */
#define SEM_ERR_PARAM_NUM           5  /* semantic error - wrong number of variables used in function call */
#define SEM_ERR_OTHER				6  /* other semantic errors */
#define ERR_ZERO_DIV				9  /* division with 0 */
#define ERR_INTERNAL				99 /* internal error */

#endif