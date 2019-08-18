#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include "scanner.h"
#include "errors.h"

char *keywords[9] = {"def", "do", "else", "end", "if", "not", "nil", "then", "while"};

int iskeyword(char *buffer)
{
    for (int i = 0; i < 9; i++)
    {
        if (strcmp(buffer, keywords[i]) == 0)
        {
            return i + 5;
        }
    }
    return 0;
}

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

struct TToken construct_token(char ungetChar, char *buffer, TType type) {

    TType kwtype;
    TOKEN newToken;

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
    newToken.type = type;

    if (type == TINT_VALUE) {
        char *ptr;
        //token.val.ival = strtol(buffer, NULL, 10);
        newToken.val.ival = strtol(buffer, &ptr, 10);

    } else if (type == TFLOAT_VALUE) {
        newToken.val.fval = strtof(buffer, NULL);

    } else if (type == TID || type == TIDFnc || type == TSTRING_VALUE) {
        newToken.val.p_cval = buffer;
        newToken.name = buffer;

    } else {
        newToken.name = NULL;
    }

    return newToken;
}

TOKEN get_token() {
    static int debug = 0;
    static int lineNum = 0; //cislo riadku
    static int linePos = 0; //pozicia na radku
    int allocated = 0;
    free(buffer);

    count = 0;
    buffer = NULL;
    TState state = START; //pociatocny stav

    TOKEN token;

    while (1) {
        // alokovanie bufferu
        buffer = realloc(buffer, (size_t) ++allocated);

        if (state != ERROR) {
            // end of file, ak je na zaciatku a zaroven dostane enter
            if (((c = (char)getchar()) == EOF) && (state == START)) {
                token.type = TEOF;
                return token;
            }

            linePos++;			 //cislovanie pozic

            // zapisovanie do bufferu bez komentarov
            if (!((state == block_cmnt || state == block_cmnt_end) && c != '=')) {
                buffer[count++] = c; //ukladanie znaku do bufferu
            }
        } else {
            token.type = TERR;
            return token;
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
                    return token;
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
                    return construct_token(c, buffer, TINT_VALUE);
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
                    return construct_token(c, buffer, TFLOAT_VALUE);
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
                    return construct_token(c, buffer, TFLOAT_VALUE);
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
                    return construct_token(c, buffer, TID);
                }
            case IDFnc:
                // identifikator funkcie
                return construct_token(c, buffer, TIDFnc);

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
                return construct_token(c, buffer, TSTRING_VALUE);

            case add:
                return construct_token(c, buffer, Tadd);


            case sub:
                return construct_token(c, buffer, Tsub);


            case mul:
                return construct_token(c, buffer, Tmul);


            case sdiv:
                return construct_token(c, buffer, Tdiv);


            case lesser:
                if (c == '=')
                {
                    state = lesserequal;
                    break;
                }
                else
                {
                    return construct_token(c, buffer, Tlesser);
                }


            case greater:
                if (c == '=')
                {
                    state = greaterequal;
                    break;
                }
                else
                {
                    return construct_token(c, buffer, Tgreater);
                }


            case lesserequal:
                return construct_token(c, buffer, Tlesserequal);


            case greaterequal:
                return construct_token(c, buffer, Tgreaterequal);


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
                            return token;
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
                    return token;
                }
                break;

            case equal:
                return construct_token(c, buffer, Tequal);


            case Snotequal:
                if (c == '=') {
                    state = notequal;
                    break;
                } else
                    state = ERROR;
                break;

            case notequal:
                return construct_token(c, buffer, Tnotequal);


            case carka:
                return construct_token(c, buffer, Tcarka);


            case assignment:
                return construct_token(c, buffer, Tassignment);


            case left_bracket:
                return construct_token(c, buffer, Tleft_bracket);


            case right_bracket:
                return construct_token(c, buffer, Tright_bracket);


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
                return token;


            default:
                state = ERROR;
                break;

        }
    }
}