/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file posfix_func.c
 * @author xhladi23 Martin Hladis
 * @author xprasi06 Matej Prasil
 * @author xvadur04 Martin Vadura
 * @author xsleza20 Alexandra Slezakova
 */

#include "posfix_func.h"

void stackInit ( NStack *s )
{
    s->top = -1;
}
int stackEmpty ( const NStack* s )
{
    return (s->top == -1);
}
int stackFull ( const NStack* s ){
    return (s->top == (TAB_SIZE - 1));
}
Nnode stackTop ( const NStack* s){
    if (stackEmpty(s) == 1)
        return NULL;
    else
        return s->nstack[s->top];
}

void stackPop ( NStack* s )
{
    if(s->top != -1)
    {
        s->nstack[s->top] = NULL;
        s->top -= 1;
    }
}
void stackPush ( NStack* s, Nnode node ){
    if((s->top == TAB_SIZE) == 0 )
    {
        s->top +=1;
        s->nstack[s->top] = node;
    }
}

void untilLeftPar ( NStack* s, NStack *out ) {

    Nnode c;
    while(stackEmpty(s) != 1) //Vyprázdění zásobníku dokud nenarazímena ( nebo zásobník se vypprázdní
    {
        c = stackTop(s);
        stackPop(s);
        if (c->data->ntype == LF_BR)
            return;
        stackPush(out ,c);
    }
}

void doOperation ( NStack* out, NStack* in, NStack* tmp,Nnode node) {

    Nnode c_top;


    if (stackEmpty(in) == 0)   //Načtení prvku z vrcholu zásobníku
    {
        c_top = stackTop(tmp);
    }
    else{
        return;
    }

    if ( node->data->ntype == LF_BR || stackEmpty(tmp) == 1 || c_top->data->ntype == LF_BR) //pokud je znak c ( vložit na zásobník nebo je zásobník prázdný lož na zásobník
    {
        stackPush(tmp, node);
        NstackPopGround(in);
    }
    else if ((node->data->ntype == DIV || node->data->ntype == DIVINIT || c_top->data->ntype == MUL) && (c_top->data->ntype == ADD || c_top->data->ntype == SUB))  //Pokud je operand nebo na vrchlu je operátor s nižší prioritou vlož na zásobník
    {
        stackPush(tmp, c_top);
    }
    else
    {                  //Pokud je c operand a na vrcholu je vyšší nebo stejná pririta
        stackPush(out,c_top);
        stackPop(tmp);
        doOperation(out, in, tmp,node);
    }
}

void infix2postfix (NStack *in,Nnode node) {

    NStack *tmp = malloc(sizeof(nStack));
    stackInit(tmp);
    NStack *out = malloc(sizeof(nStack));
    stackInit(out);

    while(in->top != -1) //Cyklus dokud nedojdeme do konce řetězce
    {
        if (in->nstack[0]->data->ntype == VAL || in->nstack[0]->data->ntype == VAR)
        {                                             //Pokud operand vložímě nakonec výstupního řetězce
            stackPush(out,in->nstack[0]);
            NstackPopGround(in);
        }
        else if (in->nstack[0]->data->ntype == MUL || in->nstack[0]->data->ntype == SUB || in->nstack[0]->data->ntype == DIV || in->nstack[0]->data->ntype == DIVINIT || in->nstack[0]->data->ntype == ADD)
        {                                             //Pokud operátor rozhodnutí co se provede s operátorem ve funkci doOperation
            doOperation(out,in,tmp,in->nstack[0]);
        }
        else if (in->nstack[0]->data->ntype == LF_BR)
        {
            stackPush(tmp, in->nstack[0]);
        }
        else if (in->nstack[0]->data->ntype == RG_BR)
        {
            untilLeftPar(tmp,out);
        }
    }

    while(tmp->top != -1)
    {
        stackPush(out, stackTop(tmp));
        NstackPopGround(tmp);
    }


    for(int i = 0; i <= out->top;i++)
    {
        node->children[node->data->child_count+i]=out->nstack[i];
    }
    node->data->child_count += out->top;

}