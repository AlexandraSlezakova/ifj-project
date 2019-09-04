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


struct TToken construct_token(char ungetChar, char *buffer, struct TToken new_token, TType type) {

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
    new_token.type = type;

    if (type == TINT_VALUE) {
        char *ptr = malloc(sizeof(char));
        //token.val.ival = strtol(buffer, NULL, 10);
        new_token.val.ival = strtol(buffer, &ptr, 10);

    } else if (type == TFLOAT_VALUE) {
        new_token.val.fval = strtof(buffer, NULL);

    } else if (type == TID) {
        new_token.val.p_cval = buffer;
        new_token.name = buffer;
    } else {
        new_token.name = NULL;
    }

    return new_token;
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

    //token = (struct TToken*) malloc(sizeof(struct TToken));
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
                        state = nula;
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
                else if (c == '+')
                {
                    state = add;
                    break;
                }
                else if (c == '-')
                {
                    state = sub;
                    break;
                }
                else if (c == '*')
                {
                    state = mul;
                    break;
                }
                else if (c == '/')
                {
                    state = sdiv;
                    break;
                }
                else if (c == '<')
                {
                    state = lesser;
                    break;
                }
                else if (c == '>')
                {
                    state = greater;
                    break;
                }
                else if (c == '!')
                {
                    state = Snotequal;
                    break;
                }
                else if (c == ',')
                {
                    state = carka;
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
                }

                else
                {
                    state = ERROR;
                    break;
                }
            case nula:
                if(isdigit(c))
                    state = ERROR;
                else
                {
                    ungetc(c, stdin);
                    state = dINT;
                }
                break;

                //cislice
            case dINT:
                if (isdigit(c))
                {
                    state = dINT;
                    break;
                }
                else if (c == '.')
                {
                    state = dbl;
                    break;
                }
                else if (c == 'e' || c == 'E')
                {
                    state = dblE;
                    break;
                }
                else
                {
                    token = construct_token(c, buffer, token, TINT_VALUE);
                    return 0;
                }

                //desetinna tecka
            case dbl:
                if (isdigit(c))
                {
                    state = dblF;
                    break;
                }
                else
                    state = ERROR;
                break;

                //cislo za desetinnou teckou
            case dblF:
                if (isdigit(c))
                {
                    state = dblF;
                    break;
                }
                else if (c == 'e' || c == 'E')
                {
                    state = dblE;
                    break;
                }
                else
                {
                    token = construct_token(c, buffer, token, TFLOAT_VALUE);
                    return 0;
                }

                // E || e
            case dblE:
                if (c == '+' || c == '-')
                {
                    state = dblES;
                    break;
                }
                else if (isdigit(c))
                {
                    state = dblEF;
                    break;
                }
                else
                    state = ERROR;
                break;

                // znak + nebo -
            case dblES:
                if (isdigit(c))
                {
                    state = dblEF;
                    break;
                }
                else
                    state = ERROR;
                break;

                //cislo znacici hodnotu exponentu
            case dblEF:
                if (isdigit(c))
                {
                    state = dblEF;
                    break;
                }
                else
                {
                    token = construct_token(c, buffer, token, TFLOAT_VALUE);
                    return 0;
                }

                //pokracovani identifikatoru
            case ID:
                if (isdigit(c) || isalpha(c) || c == '_')
                {
                    state = ID;
                    break;
                }
                if (c == '!' || c == '?')
                { //identifikator funkce
                    state = IDFnc;
                    break;
                }
                else
                {
                    token = construct_token(c, buffer, token, TID);
                    return 0;
                }
            case IDFnc:
                // identifikator funkcie
                token = construct_token(c, buffer, token, TIDFnc);
                return 0;

            case strS:
                if (c == '\\')
                {
                    state = strESC;
                    break;
                }
                else if (c > 31 && c != 34 && c != 92)
                { /*34 - " 92 - \ */
                    state = str;
                    break;
                }
                else if (c == '"')
                {
                    state = strF;
                    break;
                }
                else
                    state = ERROR;
                break;

            case str:
                if (c > 31 && c != 34 && c != 92)
                {
                    state = str;
                    break;
                }
                else if (c == '"')
                {
                    state = strF;
                    break;
                }
                else if (c == '\\')
                {
                    state = strESC;
                    break;
                }
                else
                    state = ERROR;
                break;

                //prisel backslash
            case strESC:
                if (c == '"' || c == 'n' || c == 't' || c == 's' || c == '\\')
                {
                    state = str;
                    break;
                }
                else if (c == 'x')
                {
                    state = hexa_escape;
                    break;
                }
                else
                    state = ERROR;
                break;

                //hexa escape
            case hexa_escape: //
                if (isdigit(c) || (c >= 'A' && c <= 'F'))
                { // \xh
                    state = hexa_escape2;
                    break;
                }
                else
                    state = ERROR;
                break;

            case hexa_escape2:
                if (isdigit(c) || (c >= 'A' && c <= 'F'))
                { // \xhh
                    state = str;
                    break;
                }
                else if (c == '\0')
                { //jednomistne hexa cislo
                    state = str;
                    break;
                }
                else if (c == '"')
                {
                    state = strF;
                    break;
                }
                else if (c == '\\')
                {
                    state = strESC;
                    break;
                }
                else
                    state = ERROR;
                break;

            case strF:
                token = construct_token(c, buffer, token, TSTRING_VALUE);
                return 0;

                /*************************************************************************************/

            case add:
                token = construct_token(c, buffer, token, Tadd);
                return 0;


            case sub:
                token = construct_token(c, buffer, token, Tsub);
                return 0;


            case mul:
                token = construct_token(c, buffer, token, Tmul);
                return 0;


            case sdiv:
                token = construct_token(c, buffer, token, Tdiv);
                return 0;


            case lesser:
                if (c == '=')
                {
                    state = lesserequal;
                    break;
                }
                else
                {
                    token = construct_token(c, buffer, token, Tlesser);
                    return 0;
                }


            case greater:
                if (c == '=')
                {
                    state = greaterequal;
                    break;
                }
                else
                {
                    token = construct_token(c, buffer, token, Tgreater);
                    return 0;
                }


            case lesserequal:
                token = construct_token(c, buffer, token, Tlesserequal);
                return 0;


            case greaterequal:
                token = construct_token(c, buffer, token, Tgreaterequal);
                return 0;


            case cmnt_equal_assignment:
                if (c == '=')
                {
                    state = equal;
                    break;
                }
                else if (c == 'b' )
                {
                    if (checkcmnt_begin()) {
                        state = block_cmnt;
                        linePos += 5;
                    }
                    else
                        state = ERROR;
                    break;
                }
                else
                    ungetc(c, stdin);
                state = assignment;
                break;

            case block_cmnt:
                if (c == 'e' && buffer[linePos-2] == '=') // linePos-2 lebo indexace je od 0 ale linePos sa pocita od 1
                {
                    buffer[count++] = c;
                    if (checkcmnt_end()) {
                        linePos += 3;
                        if (c == '\n') {
                            state = START;
                            lineNum++;
                            linePos = 0;
                            token.type = TEOL;
                            return 0;
                        }
                        else state = block_cmnt_end;
                    }
                    else
                        state = ERROR;
                    break;
                }
                else
                {
                    if (c == '\n'){
                        lineNum++;
                        linePos = 0;
                        count = 0;
                        allocated = 0;
                        token.type = TEOL;
                    }
                    state = block_cmnt;
                    break;
                }


            case block_cmnt_end:
                if (c == '\n') {
                    state = START;
                    lineNum++;
                    linePos = 0;
                    token.type = TEOL;
                    return 0;
                }
                break;

            case equal:
                token = construct_token(c, buffer, token, Tequal);
                return 0;


            case Snotequal:
                if (c == '=') {
                    state = notequal;
                    break;
                } else
                    state = ERROR;
                break;

            case notequal:
                token = construct_token(c, buffer, token, Tnotequal);
                return 0;


            case carka:
                token = construct_token(c, buffer, token, Tcarka);
                return 0;


            case assignment:
                token = construct_token(c, buffer, token, Tassignment);
                return 0;


            case left_bracket:
                token = construct_token(c, buffer, token, Tleft_bracket);
                return 0;


            case right_bracket:
                token = construct_token(c, buffer, token, Tright_bracket);
                return 0;


            case lineCMNT:
                if (c == '\n')
                {
                    state = START;
                    break;
                }
                else
                {
                    state = lineCMNT;
                    break;
                }


            case ERROR:
                fprintf(stderr, "Spatny format na radku:%d (char:%d)!\n", lineNum, linePos);
                token.type = TERR;
                return 1;


            default:
                state = ERROR;
                break;

        }
    }
}