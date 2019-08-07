#include <string.h>
#include <ctype.h>
#include <memory.h>
#include <stdlib.h>
#include <stdbool.h>
#include "scanner.h"
#include "errors.h"

char *keywords[9] = {"def", "do", "else", "end", "if", "not", "nil", "then", "while"};

bool checkcmnt_begin()
{
    for (int i = 0; i < 5; ++i) {
        c = (char)getchar();
        buffer[count++] = c;
    }

    if ((!strcmp(buffer, "=begin ")) || (!strcmp(buffer, "=begin\t")) || (!strcmp(buffer, "=begin\n")))
        return true;
    else
        return false;

}

bool checkcmnt_end()
{
    for (int i = 0; i < 3; ++i) {
        c = (char)getchar();
        buffer[count++] = c;
    }

    char *pom_buffer = malloc(5);
    memcpy(pom_buffer, buffer + (count-5), 5);

    if ((!strcmp(pom_buffer, "=end ")) || (!strcmp(pom_buffer, "=end\t")) || (!strcmp(pom_buffer, "=end\n")))
        return true;
    else
        return false;

}

int iskeyword(char *buffer) {
    for (int i = 0; i < 9; i++) {
        if (strcmp(buffer, keywords[i]) == 0) {
            return i + 5;
        }
    }

    return 0;

}

struct TToken construct_token(char ungetChar, char *buffer, struct TToken token, TType type) {
    TType kwtype;
    ungetc(ungetChar, stdin);
    buffer[count - 1] = '\0';

    if (type == TID) {
        int i = 0;
        while (buffer[i] != '\0') {
            buffer[i] == (char)tolower(buffer[i]);
            i++;
        }
    }

    if ((kwtype = (TType)iskeyword(buffer))) {
        type = kwtype;
    }

    //printf("%s", buffer);
    // ulozenie typu
    token.type = type;

    if (type == TINT_VALUE) {
        token.val.ival = (int)strtol(buffer, NULL, 10);

    } else if (type == TFLOAT_VALUE) {
        token.val.fval = strtof(buffer, NULL);

    } else if (type == TID || type == TSTRING_VALUE || type == TIDFnc) {
        token.val.p_cval = buffer;
    }

    return token;
}

int get_token() {
    static int debug = 0;
    static int lineNum = 0; //cislo riadku
    static int linePos = 0; //pozicia na radku
    int allocated = 0;
    free(buffer);
    count = 0;
    buffer = NULL;
    TState state = START; //pociatocny stav

    while (1) {
        // alokovanie bufferu
        buffer = realloc(buffer, (size_t) ++allocated);

        if (state != ERROR) {
            // end of file, ak je na zaciatku a zaroven dostane enter
            if (((c = (char)getchar()) == EOF) && (state == START)) {
                token.type = TEOF;
                return TOKEN_OK;
            }

            linePos++;			 //cislovanie pozic

            // zapisovanie do bufferu bez komentarov
            if (!((state == block_cmnt || state == block_cmnt_end) && c != '=')) {
                buffer[count++] = c; //ukladanie znaku do bufferu
            }
        } else {
            token.type = TERR;
            return TOKEN_ERR;
        }

        switch (state) {
            case START:
                // zaciatok identifikatoru
                if ((c>='a' && c<='z') || (c == '_')) {
                    state = ID;
                    break;
                }  // zaciatok ciselneho literalu
                else if (isdigit(c)) {
                    // nula na zaciatku nie je povolena
                    if(c == '0')
                        state = ERROR;
                    else
                        state = dINT;
                    break;
                } // white-space znak
                else if (isspace(c) && c != '\n') {
                    state = START;
                    count = 0;
                    allocated = 0;
                    break;
                } // novy riadok
                else if (c == '\n') {
                    lineNum++;
                    linePos = 0;
                    token.type = TEOL; // end-of-line
                    return 0;
                } //zaciatok stringu
                else if (c == '"') {
                    state = strS;
                    break;
                } //riadkovy komentar
                else if (c == '#') {
                    state = lineCMNT;
                    break;
                } //zaciatok blokove komentara, priradenie alebo rovna sa
                else if (c == '=') {
                    state = cmnt_equal_assignment;
                    break;
                }
                else if (c == '(')
                {
                    state = left_bracket;
                    break;
                }
                else if (c == ')')
                {
                    state = right_bracket;
                    break;
                } // ciarka
                else if (c == ',')
                {
                    state = carka;
                    break;
                } else {
                    state = ERROR;
                    break;
                }
            case ID:
                // pokracovanie identifikatoru
                if (isdigit(c) || isalpha(c) || c == '_') {
                    state = ID;
                    break;
                } // identifikator funkcie
                else if (c == '!' || c == '?') {
                    state = IDFnc;
                    break;
                } else {
                    token = construct_token(c, buffer, token, TID);
                    return 0;
                }
            case IDFnc:
                // identifikator funkcie
                token = construct_token(c, buffer, token, TIDFnc);
                return 0;
            case dINT:
                if (isdigit(c)) {
                    state = dINT;
                    break;
                } // desatinna bodka
                else if (c == '.') {
                    state = dbl;
                    break;
                } // exponent
                else if (c == 'e' || c == 'E') {
                    state = dblE;
                    break;
                } // vytvorenie tokenu
                else {
                    token = construct_token(c, buffer, token, TINT_VALUE);
                    return 0;
                }
            case left_bracket:
                token = construct_token(c, buffer, token, Tleft_bracket);
                return 0;

            case right_bracket:
                token = construct_token(c, buffer, token, Tright_bracket);
                return 0;

            case carka:
                token = construct_token(c, buffer, token, Tcarka);
                return 0;
            // zaciatok blokoveho komentara alebo priradenie
            case cmnt_equal_assignment:
                if (c == '=') {
                    state = equal;
                    break;
                } // blokovy komentar
                else if (c == 'b' ) {
                    if (checkcmnt_begin()) {
                        state = block_cmnt;
                        linePos += 5;
                    } else
                        state = ERROR;
                    break;
                } else
                    ungetc(c, stdin);
                state = assignment;
                break;
            // priradenie
            case assignment:
                token = construct_token(c, buffer, token, Tassignment);
                return 0;
            default:
                state = ERROR;
                break;

        }

    }
}