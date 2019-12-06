/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file posfix_func.h
 * @author xhladi23 Martin Hladis
 * @author xprasi06 Matej Prasil
 * @author xvadur04 Martin Vadura
 * @author xsleza20 Alexandra Slezakova
 */

#ifndef SCANNER_C_POSFIX_FUNC_H
#define SCANNER_C_POSFIX_FUNC_H


#include "ast.h"

void stackInit ( NStack *s );
int stackEmpty ( const NStack* s );
int stackFull ( const NStack* s );
Nnode stackTop ( const NStack* s);
void stackPop ( NStack* s );
void stackPush ( NStack* s, Nnode node );
void infix2postfix (NStack *in,Nnode node);
void doOperation ( NStack* out, NStack* in, NStack *tmp, Nnode node);
void untilLeftPar ( NStack* s ,NStack *out);

#endif
