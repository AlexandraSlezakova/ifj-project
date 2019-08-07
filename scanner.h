#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <memory.h>
#include <stdbool.h>

#ifndef SCANNER_H
#define SCANNER_H

typedef enum {
    START, //pociatocny stav
    dINT,
    dbl,    // desatinna bodka
    dblE,   // exponent
    dblF,   // cislo s desatinnou bodkou
    dblES,  // nepovinne znameniko +/- za exponentom
    dblEF,  // koniec ciselneho literalu
    ID,     // identifikator
    IDFnc,  // identifikator funkcie
    strS,   // zaciatok retazca
    strESC, // escape sekvencia v retazci
    str,    // retazec
    strF,   // koniec retazca
    lineCMNT,               // riadkovy komentar
    cmnt_equal_assignment,  // znak rovnosti na zaciatok blokoveho komentara
    equal,  // znak rovnosti
    block_cmnt,     // blokovy komentar
    block_cmnt_end, // koniec blokoveho komentara
    assignment,
    add,
    sub,
    nula,
    mul,
    sdiv,
    lesser,
    greater,
    lesserequal,
    greaterequal,
    Snotequal,
    notequal,
    carka,
    left_bracket,
    right_bracket,
    hexa_escape,
    hexa_escape2,
    ERROR
}TState;

typedef enum {
    TID,			//0
    TIDFnc,			//1
    TINT_VALUE,		//2 hodnota celociselneho literalu
    TFLOAT_VALUE,	//3
    TSTRING_VALUE,	//4

    //keywords
    Tdef,			//5
    Tdo,			//6
    Telse,			//7
    Tend,			//8
    Tif,			//9
    Tnot,			//10
    Tnil,			//11
    Tthen,			//12
    Twhile,			//13

    //operatory
    Tadd,			//14
    Tsub,			//15
    Tmul,			//16
    Tdiv,			//17
    Tlesser,		//18
    Tgreater,		//19
    Tlesserequal,	//20
    Tgreaterequal,	//21
    Tequal,			//22
    Tnotequal,		//23

    //ostatni
    Tcarka,			//24
    Tassignment,	//25
    Tleft_bracket,	//26
    Tright_bracket,	//27

    //specialni
    TEOF,			//28 koniec suboru
    TEOL,			//29 koniec riadku
    TERR			//30 error
}TType;

struct TToken {
    TType type;
    union {
        int *ival;
        float *fval;
        char* p_cval; // nazov
    }val;
}token;

char c;
char c;
struct TToken;
int get_token();
struct TToken construct_token(char ungetChar, char* buffer, struct TToken token, TType type);
int iskeyword(char *hString);
char* buffer;
int count;


#endif