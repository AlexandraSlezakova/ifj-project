/**
 * Formal Languages and Compilers
 * Implementation of the imperative language interpreter
 * @file generator.c
 * @brief code generator
 * @author xhladi23 Martin Hladis
 * @author xprasi06 Matej Prasil
 * @author xvadur04 Martin Vadura
 * @author xsleza20 Alexandra Slezakova
 */

#include "generator.h"


bool func;

// Function of detection how many numbers are in variable
int lenHelper(unsigned long long x) {
    if (x >= 1000000000) return 10;
    if (x >= 100000000)  return 9;
    if (x >= 10000000)   return 8;
    if (x >= 1000000)    return 7;
    if (x >= 100000)     return 6;
    if (x >= 10000)      return 5;
    if (x >= 1000)       return 4;
    if (x >= 100)        return 3;
    if (x >= 10)         return 2;
    return 1;
}

// Function of generating unique label
char * generate_unique_label()
{
    char prefix = '$';
    static unsigned long long int i = 0;
    int lenght = lenHelper(i);
    char *label = malloc((lenght + 2) * sizeof(char));
    label[0] = prefix;
    sprintf(&label[1], "%llu", i++);
    return label;
}

// Function of generating unique variable identifier
char * generate_unique_identifier()
{
    char prefix = '%';
    static unsigned long long int i = 0;
    int lenght = lenHelper(i);
    char *ident = malloc((lenght + 2) * sizeof(char));
    ident[0] = prefix;
    sprintf(&ident[1], "%llu", i++);
    return ident;
}


// Function of generating unique params in function
char * generate_unique_func_identifier( int a, char c)
{
    static unsigned long long int i = 0;
    if (a == 0)  //Restart to zero
    {
        i=0;
        return NULL;
    }
    char prefix = '%';

    int lenght = lenHelper(i);
    char *ident = malloc((lenght + 4) * sizeof(char));
    ident[0] = prefix;
    sprintf(&ident[1], "%c", c);
    sprintf(&ident[2], "_");
    sprintf(&ident[3], "%llu", i++);
    return ident;
}

// Creating local frame after start
void create_local_frame()
{
    fprintf(stdout, "CREATEFRAME\n");
    fprintf(stdout, "PUSHFRAME\n");
}

// End main frame
void end_frame()
{
    fprintf(stdout, "POPFRAME\n");
}

// Create Header and jump on main
void create_header()
{
    fprintf(stdout, ".IFJcode19\n");
    fprintf(stdout, "JUMP $$main\n");
}

// Save value into variable of unieque identifier
void save_val_to_variable(Nnode ast, bool TF)
{

    char *newvar = generate_unique_identifier();

    // Decition to which frame save variable
    if (TF == 1) {
        fprintf(stdout, "DEFVAR TF@%s\n", newvar);
        fprintf(stdout, "MOVE TF@%s %s\n", newvar, ast->data->data);

    } else {
        if (ast->data->inmain) {
            fprintf(stdout, "DEFVAR GF@%s\n", newvar);
            fprintf(stdout, "MOVE GF@%s %s\n", newvar, ast->data->data);
        }
        else {
            fprintf(stdout, "DEFVAR LF@%s\n",newvar);
            fprintf(stdout, "MOVE LF@%s %s\n", newvar, ast->data->data);
        }
    }


    ast->data->data = newvar;
}


// FUnction of inicialization variables into type_contro_exit
void generate_type_control_exit(Nnode ast)
{
    fprintf(stdout, "\n");
    fprintf(stdout, "CREATEFRAME\n");
    fprintf(stdout, "DEFVAR TF@%%1\n");
    fprintf(stdout, "DEFVAR TF@%%2\n");

    if (ast->data->inmain) {
        fprintf(stdout, "MOVE TF@%%1 GF@%s\n", ast->children[0]->data->data);
        fprintf(stdout, "MOVE TF@%%2 GF@%s\n", ast->children[1]->data->data);
    } else {
        fprintf(stdout, "MOVE TF@%%1 LF@%s\n", ast->children[0]->data->data);
        fprintf(stdout, "MOVE TF@%%2 LF@%s\n", ast->children[1]->data->data);
    }

    // Call function which control types
    fprintf(stdout, "CALL $contr_exit\n");

    // Return changed values into variables
    if (ast->data->inmain) {
        fprintf(stdout, "MOVE GF@%s TF@%%1\n", ast->children[0]->data->data);
        fprintf(stdout, "MOVE GF@%s TF@%%2\n", ast->children[1]->data->data);
    } else {
        fprintf(stdout, "MOVE LF@%s TF@%%1\n", ast->children[0]->data->data);
        fprintf(stdout, "MOVE LF@%s TF@%%2\n", ast->children[1]->data->data);
    }

    fprintf(stdout, "\n");
}


// Decition which operation print
void aritmetic_operation(Nnode ast)
{
    switch(ast->data->ntype) {
        case ADD:
            fprintf(stdout, "ADD");
            break;
        case SUB:
            fprintf(stdout, "SUB");
            break;
        case MUL:
            fprintf(stdout, "MUL");
            break;
        case DIV:
            fprintf(stdout, "DIV");
            break;
        case GR:
            fprintf(stdout, "GT");
            break;
        case LESS:
            fprintf(stdout, "LT");
            break;
        case COMP:
            fprintf(stdout, "EQ");
            break;
        case NOTCOMP:
            fprintf(stdout, "NOTCOMP");
            break;
        case GEQ:
            fprintf(stdout, "GT");
            break;
        case LOQ:
            fprintf(stdout, "LT");
            break;
        case DIVINIT:
            fprintf(stdout, "IDIV");
            break;
        default:
            break;
    }
}

// Fuction of inicialization variables into function which control type of variables
void generate_type_control_exit2(Nnode child1, Nnode child2)
{
    fprintf(stdout, "\n");
    fprintf(stdout, "CREATEFRAME\n");
    fprintf(stdout, "DEFVAR TF@%%1\n");
    fprintf(stdout, "DEFVAR TF@%%2\n");

    if (func == true) {
        fprintf(stdout, "MOVE TF@%%1 LF@%s\n", child1->data->data);
        fprintf(stdout, "MOVE TF@%%2 LF@%s\n", child2->data->data);
    } else {
        fprintf(stdout, "MOVE TF@%%1 GF@%s\n", child1->data->data);
        fprintf(stdout, "MOVE TF@%%2 GF@%s\n", child2->data->data);
    }
    // Call function which control types variables
    fprintf(stdout, "CALL $contr_exit\n");

    // Save changed values
    if (func == true) {
        fprintf(stdout, "MOVE LF@%s TF@%%1\n", child1->data->data);
        fprintf(stdout, "MOVE LF@%s TF@%%2\n", child2->data->data);
    } else {
        fprintf(stdout, "MOVE GF@%s TF@%%1\n", child1->data->data);
        fprintf(stdout, "MOVE GF@%s TF@%%2\n", child2->data->data);
    }

    fprintf(stdout, "\n");
}

// Fucntion which generate math operation
void generate_math_aritmetic(Nnode ast)
{
    int before = 0;
    fprintf(stdout, "CREATEFRAME\n");
    // Save input variable or values into new temporary frame
    for (int i = 1; ast->children[i] != NULL; i ++){

        if (ast->children[i]->data->ntype == VAL) {
            save_val_to_variable(ast->children[i], true);
        } else
        if (ast->children[i]->data->ntype == VAR) {
            char *new_name = generate_unique_identifier();

            if (func == true) {
                fprintf(stdout, "DEFVAR TF@%s\n", new_name);
                fprintf(stdout, "MOVE TF@%s LF@%s\n", new_name, ast->children[i]->data->data);
            } else {
                fprintf(stdout, "DEFVAR TF@%s\n", new_name);
                fprintf(stdout, "MOVE TF@%s GF@%s\n", new_name, ast->children[i]->data->data);
            }
            ast->children[i]->data->data = new_name;
        }

    }
    before = func;
    func = true;

    fprintf(stdout, "DEFVAR TF@%%RETVAL\n");
    fprintf(stdout, "PUSHFRAME\n");
    //Math operation
    // Search for operator in array of operands and operators saved in postfix order
    for(int i = 1; ast->children[i] != NULL; i++ )
    {
        //Generate unique identifiers and labels
        char *new = generate_unique_identifier();
        fprintf(stdout, "DEFVAR LF@%s\n", new);
        char *not_int = generate_unique_label();
        char *not_flout = generate_unique_label();
        char *div_not_0 = generate_unique_label();
        char *div_not_0_0 = generate_unique_label();
        char *var1_str = generate_unique_label();
        char *var2_str = generate_unique_label();
        char *end_add = generate_unique_label();

        Nnode c1,c2,o;
        int j = 1;
        for(; ast->children[j] != NULL; j++)
        {
            // Find operator on j position in array and 2 operands on j-1 and j-2 position
            if(ast->children[j]->data->ntype == ADD || ast->children[j]->data->ntype == SUB || ast->children[j]->data->ntype == MUL
               || ast->children[j]->data->ntype == DIV || ast->children[j]->data->ntype == DIVINIT)
            {

                o = ast->children[j];
                c1 = ast->children[j - 2];
                c2 = ast->children[j - 1];
                generate_type_control_exit2(c1, c2);

                //Decition which operation compute
                if (o->data->ntype == DIV) {
                    fprintf(stdout, "DEFVAR TF@%s\n", not_flout);
                    fprintf(stdout, "DEFVAR TF@%s\n", div_not_0_0);
                    fprintf(stdout, "TYPE TF@%s LF@%s\n", not_flout, c1->data->data);
                    fprintf(stdout, "TYPE TF@%s LF@%s\n", div_not_0_0, c2->data->data);
                    fprintf(stdout, "JUMPIFNEQ %s TF@%s string@float\n", not_flout, not_flout);
                    fprintf(stdout, "JUMPIFNEQ %s TF@%s string@float\n", not_flout, div_not_0_0);
                    fprintf(stdout, "JUMPIFNEQ %s  LF@%s float@0x0p+0\n", div_not_0_0, c2->data->data);
                    fprintf(stdout, "EXIT int@9\n");
                    fprintf(stdout, "JUMP %s\n", div_not_0_0);
                    fprintf(stdout, "LABEL %s\n", not_flout);
                    fprintf(stdout, "EXIT int@4\n");
                    fprintf(stdout, "LABEL %s\n", div_not_0_0);
                    fprintf(stdout, "DIV LF@%s LF@%s LF@%s\n", new, c1->data->data, c2->data->data);
                } else if (o->data->ntype == DIVINIT){

                    fprintf(stdout, "DEFVAR TF@%s\n", not_int);
                    fprintf(stdout, "DEFVAR TF@%s\n", div_not_0);
                    fprintf(stdout, "TYPE TF@%s LF@%s\n", not_int, c1->data->data);
                    fprintf(stdout, "TYPE TF@%s LF@%s\n", div_not_0, c2->data->data);
                    fprintf(stdout, "JUMPIFNEQ %s TF@%s string@int\n", not_int, not_int);
                    fprintf(stdout, "JUMPIFNEQ %s TF@%s string@int\n", not_int, div_not_0);
                    fprintf(stdout, "JUMPIFNEQ %s LF@%s int@0\n", div_not_0, c2->data->data);
                    fprintf(stdout, "EXIT int@9\n");
                    fprintf(stdout, "JUMP %s\n", div_not_0);
                    fprintf(stdout, "LABEL %s\n", not_int);
                    fprintf(stdout, "EXIT int@4\n");
                    fprintf(stdout, "LABEL %s\n", div_not_0);
                    fprintf(stdout, "IDIV LF@%s LF@%s LF@%s\n", new, c1->data->data, c2->data->data);
                } else if (o->data->ntype == ADD) {
                    fprintf(stdout, "DEFVAR TF@%s\n", var1_str);
                    fprintf(stdout, "DEFVAR TF@%s\n", var2_str);
                    fprintf(stdout, "TYPE TF@%s LF@%s\n", var1_str, c1->data->data);
                    fprintf(stdout, "TYPE TF@%s LF@%s\n", var2_str, c2->data->data);
                    fprintf(stdout, "JUMPIFEQ  %s TF@%s string@string\n", var1_str, var1_str);
                    fprintf(stdout, "ADD LF@%s LF@%s LF@%s\n", new, c1->data->data, c2->data->data);
                    fprintf(stdout, "JUMP %s\n", end_add);
                    fprintf(stdout, "LABEL %s\n", var1_str);
                    fprintf(stdout, "JUMPIFEQ %s TF@%s string@string\n", var2_str, var2_str);
                    fprintf(stdout, "EXIT int@4\n");
                    fprintf(stdout, "JUMP %s\n", end_add);
                    fprintf(stdout, "LABEL %s\n", var2_str);
                    fprintf(stdout, "CONCAT LF@%s LF@%s LF@%s\n", new, c1->data->data, c2->data->data);
                    fprintf(stdout, "LABEL %s\n", end_add);
                } else {
                    fprintf(stdout, "DEFVAR TF@%s\n", var1_str);
                    fprintf(stdout, "TYPE TF@%s LF@%s\n", var1_str, c1->data->data);
                    fprintf(stdout, "JUMPIFEQ %s TF@%s string@string\n", var1_str, var1_str);
                    fprintf(stdout, "JUMPIFEQ %s TF@%s string@bool\n", var1_str, var1_str);
                    fprintf(stdout, "JUMP %s\n", end_add);
                    fprintf(stdout, "LABEL %s\n", var1_str);
                    fprintf(stdout, "EXIT int@4\n");
                    fprintf(stdout, "LABEL %s\n", end_add);
                    aritmetic_operation(o);
                    fprintf(stdout, " LF@%s LF@%s LF@%s\n", new, c1->data->data, c2->data->data);

                }
                //Save new compute value on operator position
                ast->children[j-2]->data->data = new;
                //Move values in array on position of operand
                while(ast->children[j] != NULL)
                {
                    ast->children[j-1]=ast->children[j+1];
                    j++;
                }
                // NULL last child
                ast->children[j-1] = NULL;
                //restart value to search for new operator
                i = 1;
                break;

            }
        }

    }

    fflush(stdout);
    fprintf(stdout, "MOVE LF@%%RETVAL LF@%s\n", ast->children[1]->data->data);
    fprintf(stdout, "POPFRAME\n");

    //func = 0;
    if (before == false)
        func = false;

}




// Function which find out type of variable
char *get_data_type(HTItem *item)
{
    char *type;
    switch (item->data_type) {
        case TYPE_STRING:
            type = "string@";
            break;
        case TYPE_FLOAT:
            type = "float@";
            break;
        case TYPE_IDENTIFIER:
            type = "int@";
            break;
        case TYPE_NIL:
            type = "nil@";
            break;
        default:
            type = NULL;
            break;
    }

    return type;
}



// Function which generate vardef of new variable
int generate_vardef( Nnode ast)
{

    if (ast->data->inmain){

        fprintf(stdout, "DEFVAR GF@%s\n",  ast->children[0]->data->data);
    } else {
        fprintf(stdout, "DEFVAR LF@%s\n",  ast->children[0]->data->data);
    }

    return OK;
}

char* memory_model(Nnode ast)
{
    if(ast->parent_node == root || ast->data->indent == 0 || ast->data->inmain == true)
        return "GF";
    else
        return "LF";
}

// FUnction which assign values or variables, result of function, into another variable
int generate_assign(Nnode ast)
{

    if (ast->data->inmain) {

        // If variable we want to assign to is first time use generating variable definition
        if (ast->children[0]->data->ntype == VAR_DEF) {

            generate_vardef( ast);
        }

        // if value which we  want assign is generate by function call
        if(ast->children[1]->data->ntype == CALL)
        {

            indetify_call_function(ast->children[1]);
            if (func == true) {
                fprintf(stdout, "MOVE LF@%s TF@%%RETVAL\n", ast->children[0]->data->data);
            } else {
                fprintf(stdout, "MOVE GF@%s TF@%%RETVAL\n", ast->children[0]->data->data);
            }
            return 0;
        }
        if (ast->children[1] == NULL)
            return 0;

        // if value which we want assign is matematic operation
        if (ast->children[1] != NULL && ast->children[2] != NULL && ast->children[3] != NULL) {
            generate_math_aritmetic(ast);
            fprintf(stdout, "MOVE GF@%s TF@%%RETVAL\n", ast->children[0]->data->data);
            return 0;
            //  if value which we want assign is another variable
        } else if (ast->children[1]->data->ntype == VAR) {
            if (func == true) {
                fprintf(stdout, "MOVE LF@%s LF@%s\n", ast->children[0]->data->data, ast->children[1]->data->data);
            } else {
                fprintf(stdout, "MOVE GF@%s GF@%s\n", ast->children[0]->data->data, ast->children[1]->data->data);
            }
            return 0;
        }
        //  if value which we want assign is value
        fprintf(stdout, "MOVE GF@%s %s\n", ast->children[0]->data->data, ast->children[1]->data->data);

    } else {
        // If variable we want to assign to is first time use generating variable definition
        if (ast->children[0]->data->ntype == VAR_DEF) {
            generate_vardef( ast);
        }
        // if value which we  want assign is generate by function call
        if (ast->children[1]->data->ntype == CALL)
        {
            indetify_call_function(ast->children[1]);
            if (func == true) {
                fprintf(stdout, "MOVE LF@%s TF@%%RETVAL\n", ast->children[0]->data->data);
            } else {
                fprintf(stdout, "MOVE GF@%s TF@%%RETVAL\n", ast->children[0]->data->data);
            }
            return 0;
        }

        if (ast->children[1] == NULL)
            return 0;
        // if value which we want assign is matematic operation
        if (ast->children[1] != NULL && ast->children[2] != NULL && ast->children[3] != NULL) {
            generate_math_aritmetic(ast);
            fprintf(stdout, "MOVE LF@%s TF@%%RETVAL\n", ast->children[0]->data->data);
            return 0;
            //  if value which we want assign is another variable
        } else if (ast->children[1]->data->ntype == VAR) {
            if (func == true) {
                fprintf(stdout, "MOVE LF@%s LF@%s\n", ast->children[0]->data->data, ast->children[1]->data->data);
            } else {
                fprintf(stdout, "MOVE GF@%s GF@%s\n", ast->children[0]->data->data, ast->children[1]->data->data);
            }
            return 0;
        }
        //  if value which we want assign is value
        fprintf(stdout, "MOVE LF@%s %s\n", ast->children[0]->data->data, ast->children[1]->data->data);
    }

    return 0;
}




// Fuction of inicialization variables into function which control type of variables
void generate_type_control_cmp(Nnode ast)
{
    fprintf(stdout, "CREATEFRAME\n");
    fprintf(stdout, "DEFVAR TF@%%1\n");
    fprintf(stdout, "DEFVAR TF@%%2\n");

    if (ast->data->inmain) {
        fprintf(stdout, "MOVE TF@%%1 GF@%s\n", ast->children[0]->data->data);
        fprintf(stdout, "MOVE TF@%%2 GF@%s\n", ast->children[1]->data->data);
    } else {
        fprintf(stdout, "MOVE TF@%%1 LF@%s\n", ast->children[0]->data->data);
        fprintf(stdout, "MOVE TF@%%2 LF@%s\n", ast->children[1]->data->data);
    }

    fprintf(stdout, "CALL $contr_cmp\n");

}
// Function which control type of variables
void type_control()
{
    fprintf(stdout, "\n");
    fprintf(stdout, "LABEL $contr_cmp\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@cmp1\n");
    fprintf(stdout, "DEFVAR LF@cmp2\n");
    fprintf(stdout, "DEFVAR LF@%%RETVAL\n");

    fprintf(stdout, "TYPE LF@cmp1 LF@%%1\n");
    fprintf(stdout, "TYPE LF@cmp2 LF@%%2\n");

    fprintf(stdout, "JUMPIFEQ same_type LF@cmp1 LF@cmp2\n");
    fprintf(stdout, "JUMPIFEQ same_type LF@cmp1 nil@nil\n");
    fprintf(stdout, "JUMPIFEQ same_type LF@cmp2 nil@nil\n");
    fprintf(stdout, "MOVE LF@%%RETVAL bool@false\n");
    fprintf(stdout, "JUMP contr_end\n");
    fprintf(stdout, "LABEL same_type\n");

    fprintf(stdout, "MOVE LF@%%RETVAL bool@true\n");
    fprintf(stdout, "LABEL contr_end\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n\n");

}


//Funciton which control type of variable and if is wrong then exit
void type_control_exit()
{
    fprintf(stdout, "\n");
    fprintf(stdout, "LABEL $contr_exit\n");
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@cmp1\n");
    fprintf(stdout, "DEFVAR LF@cmp2\n");
    fprintf(stdout, "DEFVAR LF@%%RETVAL\n");
    fprintf(stdout, "DEFVAR LF@convert\n");
    fprintf(stdout, "TYPE LF@cmp1 LF@%%1\n");
    fprintf(stdout, "TYPE LF@cmp2 LF@%%2\n");
    fprintf(stdout, "JUMPIFEQ same_type_exit LF@cmp1 LF@cmp2\n");
    fprintf(stdout, "JUMPIFEQ wrong_type LF@cmp1 string@string\n");
    fprintf(stdout, "JUMPIFEQ wrong_type LF@cmp2 string@string\n");
    fprintf(stdout, "JUMPIFEQ wrong_type LF@cmp1 nil@nil\n");
    fprintf(stdout, "JUMPIFEQ wrong_type LF@cmp2 nil@nil\n");
    fprintf(stdout, "JUMPIFEQ wrong_type LF@cmp1 string@bool\n");
    fprintf(stdout, "JUMPIFEQ wrong_type LF@cmp2 string@bool\n");
    fprintf(stdout, "JUMPIFEQ convert_first LF@cmp1 string@int\n");
    fprintf(stdout, "MOVE LF@convert LF@%%2\n");
    fprintf(stdout, "INT2FLOAT LF@%%2 LF@convert\n");
    fprintf(stdout, "JUMP same_type_exit\n");

    fprintf(stdout, "LABEL convert_first\n");
    fprintf(stdout, "MOVE LF@convert LF@%%1\n");
    fprintf(stdout, "INT2FLOAT LF@%%1 LF@convert\n");
    fprintf(stdout, "JUMP same_type_exit\n");

    fprintf(stdout, "LABEL wrong_type\n");
    fprintf(stdout, "MOVE LF@%%RETVAL bool@false\n");
    fprintf(stdout, "EXIT int@4\n");
    fprintf(stdout, "LABEL same_type_exit\n");
    fprintf(stdout, "MOVE LF@%%RETVAL bool@true\n");
    fprintf(stdout, "LABEL end_control_exit\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n\n");
}


//Function which generate logical and comparable operation
int generate_aritmetic(Nnode ast)
{
    char *new = generate_unique_identifier();
    fprintf(stdout, "CREATEFRAME\n");
    // Condition have only one value
    if (ast->children[0] != NULL && ast->children[1] == NULL && ast->children[2] == NULL) {
        char *pom = generate_unique_identifier();
        char *type = generate_unique_identifier();
        char *end = generate_unique_label();
        if (func == true) {
            fprintf(stdout, "DEFVAR TF@%s\n", ast->children[0]->data->data);
            fprintf(stdout, "MOVE TF@%s LF@%s\n", ast->children[0]->data->data, ast->children[0]->data->data);
        } else {
            fprintf(stdout, "DEFVAR TF@%s\n", ast->children[0]->data->data);
            fprintf(stdout, "MOVE TF@%s GF@%s\n", ast->children[0]->data->data, ast->children[0]->data->data);
        }
        // Condition have to be more or equal 1 to evaluate true
        fprintf (stdout, "PUSHFRAME\n");
        fprintf(stdout, "DEFVAR LF@%s\n", type);
        fprintf(stdout, "DEFVAR LF@%s\n", new);
        fprintf(stdout, "DEFVAR LF@%s\n", pom);
        fprintf(stdout, "TYPE LF@%s LF@%s\n", type, ast->children[0]->data->data);
        fprintf(stdout, "JUMPIFEQ %s LF@%s string@int\n", end, type);
        fprintf(stdout, "EXIT int@4\n");
        fprintf(stdout, "LABEL %s\n", end);
        fprintf(stdout, "GT LF@%s LF@%s int@1\n", new, ast->children[0]->data->data);
        fprintf(stdout, "EQ LF@%s LF@%s int@1\n", pom, ast->children[0]->data->data);
        fprintf(stdout, "OR LF@%s LF@%s LF@%s\n", new, pom, new);
        ast->children[0]->data->data = new;

        fprintf(stdout, "POPFRAME\n");


    } else { //Condition have two values

        if (ast->children[0]->data->ntype == VAR){
            if (func == true) {
                fprintf(stdout, "DEFVAR TF@%s\n", ast->children[0]->data->data);
                fprintf(stdout, "MOVE TF@%s LF@%s\n", ast->children[0]->data->data, ast->children[0]->data->data);
            } else {
                fprintf(stdout, "DEFVAR TF@%s\n", ast->children[0]->data->data);
                fprintf(stdout, "MOVE TF@%s GF@%s\n", ast->children[0]->data->data, ast->children[0]->data->data);
            }
        }
        if (ast->children[1]->data->ntype == VAR){
            if (func == true) {
                fprintf(stdout, "DEFVAR TF@%s\n", ast->children[1]->data->data);
                fprintf(stdout, "MOVE TF@%s LF@%s\n", ast->children[1]->data->data, ast->children[1]->data->data);
            } else {
                fprintf(stdout, "DEFVAR TF@%s\n", ast->children[1]->data->data);
                fprintf(stdout, "MOVE TF@%s GF@%s\n", ast->children[1]->data->data, ast->children[1]->data->data);
            }

        }

        fprintf (stdout, "PUSHFRAME\n");

        bool before;

        if (ast->children[0]->data->ntype == VAL) {
            ast->children[0]->data->inmain = false;
            save_val_to_variable(ast->children[0], false);
        }

        if (ast->children[1]->data->ntype == VAL) {
            ast->children[1]->data->inmain = false;
            save_val_to_variable(ast->children[1], false);
        }
        fprintf(stdout, "DEFVAR LF@%s\n", new);
        //Choose which operation do
        if (ast->children[2]->data->ntype == COMP || ast->children[2]->data->ntype == NOTCOMP) {
            before = ast->data->inmain;
            ast->data->inmain = false;
            generate_type_control_cmp(ast);
            ast->data->inmain = before;

            fprintf(stdout, "EQ LF@%s LF@%s LF@%s\n", new, ast->children[0]->data->data, ast->children[1]->data->data);
            fprintf(stdout, "AND LF@%s TF@%%RETVAL LF@%s\n",  new, new);
            if (ast->children[2]->data->ntype == NOTCOMP)
                fprintf(stdout, "NOT LF@%s LF@%s\n", new, new);
        }
        else {
            before = ast->data->inmain;
            ast->data->inmain = false;
            generate_type_control_exit(ast);
            ast->data->inmain = before;
            char *type = generate_unique_identifier();
            char *end = generate_unique_label();
            fprintf(stdout, "DEFVAR LF@%s\n", type);
            fprintf(stdout, "TYPE LF@%s LF@%s\n", type, ast->children[0]->data->data);
            fprintf(stdout, "JUMPIFNEQ %s LF@%s string@string\n", end, type);
            fprintf(stdout, "EXIT int@4\n");
            fprintf(stdout, "LABEL %s\n",end );
            aritmetic_operation(ast->children[2]);
            fprintf(stdout, " LF@%s LF@%s LF@%s\n", new, ast->children[0]->data->data, ast->children[1]->data->data);
            fprintf(stdout, "AND LF@%s TF@%%RETVAL LF@%s\n", new, new);
            if (ast->children[2]->data->ntype == LOQ){
                fprintf(stdout, "EQ TF@%%RETVAL LF@%s LF@%s\n", ast->children[0]->data->data, ast->children[1]->data->data);
                fprintf(stdout, "OR LF@%s TF@%%RETVAL LF@%s\n", new, new);
            } else if (ast->children[2]->data->ntype == GEQ) {
                fprintf(stdout, "EQ TF@%%RETVAL LF@%s LF@%s\n", ast->children[0]->data->data, ast->children[1]->data->data);
                fprintf(stdout, "OR LF@%s TF@%%RETVAL LF@%s\n", new, new);
            }
            // }
        }
        ast->children[0]->data->data = new;
        fprintf(stdout, "POPFRAME\n");
    }
    return 0;
}

//Funciton to generate if statement
int generate_if(Nnode ast, HTable *table)
{
    char *label_else = generate_unique_label();
    char *label_end = generate_unique_label();
    //Find variables which are first use into program
    if (func == true) {
        for(int i = 0; ast->children[1]->children[i] != NULL; i++) {
            if (ast->children[1]->children[i]->data->ntype == ASSIGN) {
                if (ast->children[1]->children[i]->children[0]->data->ntype == VAR_DEF) {
                    fprintf(stdout, "DEFVAR LF@%s\n", ast->children[1]->children[i]->children[0]->data->data);
                    ast->children[1]->children[i]->children[0]->data->ntype = VAR;
                }
            }

        }
        if (ast->children[2] != NULL) {
            for(int i = 0; ast->children[2]->children[i] != NULL; i++) {
                if (ast->children[2]->children[i]->data->ntype == ASSIGN) {
                    if (ast->children[2]->children[i]->children[0]->data->ntype == VAR_DEF) {
                        fprintf(stdout, "DEFVAR LF@%s\n", ast->children[2]->children[i]->children[0]->data->data);
                        ast->children[2]->children[i]->children[0]->data->ntype = VAR;
                    }
                }

            }
        }
    }
    //Generate condition
    generate(ast->children[0], table);
    //Decition to do true or false body of if
    if(ast->data->inmain){
        fprintf(stdout, "JUMPIFNEQ %s TF@%s bool@true\n", label_else, ast->children[0]->children[0]->data->data);
    } else {
        fprintf(stdout, "JUMPIFNEQ %s TF@%s bool@true\n", label_else, ast->children[0]->children[0]->data->data);

    }
    // Generate body of if
    generate(ast->children[1], table);
    fprintf(stdout, "JUMP %s\n", label_end);
    fprintf(stdout, "LABEL %s\n", label_else);
    if (ast->children[2] != NULL)
        generate(ast->children[2], table);
    fprintf(stdout, "LABEL %s\n", label_end);

    return 0;
}

// function to find and vardef variables which are first use into body of if or while
void vardef_before(Nnode ast)
{
    if(ast->data->data != NULL) {
        if (ast->data->ntype == VAR_DEF) {
            if (func == true) {

                fprintf(stdout, "DEFVAR LF@%s\n", ast->data->data);
            } else {
                fprintf(stdout, "DEFVAR GF@%s\n", ast->data->data);
            }
            ast->data->ntype = VAR;
        }
    }


    for(int i = 0; ast->data->child_count >= i; i++)
    {
        if (ast->children[i]!=NULL)
            vardef_before(ast->children[i]);
    }
}
// function to while statement
int generate_while(Nnode ast,HTable *table)
{
    char *label_start = generate_unique_label();
    char *label_end = generate_unique_label();

    vardef_before(ast->children[1]);

    fprintf(stdout, "LABEL %s\n", label_start);
    //Generate condition of while
    generate(ast->children[0], table);
    fprintf(stdout, "JUMPIFNEQ %s TF@%s bool@true\n", label_end, ast->children[0]->children[0]->data->data);
    //Generate body of while
    generate(ast->children[1], table);
    fprintf(stdout, "JUMP %s\n", label_start);
    fprintf(stdout, "LABEL %s\n", label_end);
    return 0;
}

// Generate call of the function
void generate_call(Nnode ast)
{
    //Reset to 0 indetifier
    generate_unique_func_identifier(0,'F');
    // Generate all params of function
    if (ast->children[0] != NULL) {
        for(int i = 0; ast->children[0]->children[i] != NULL; i++)
        {
            char *newvar = generate_unique_func_identifier(1,'F');
            fprintf(stdout, "DEFVAR TF@%s\n", newvar);
            if (ast->children[0]->children[i]->data->ntype == VAL) {
                fprintf(stdout, "MOVE TF@%s %s\n", newvar, ast->children[0]->children[i]->data->data);
            } else {


                if (func == true) {
                    fprintf(stdout, "MOVE TF@%s LF@%s\n", newvar, ast->children[0]->children[i]->data->data);
                } else {
                    fprintf(stdout, "MOVE TF@%s GF@%s\n", newvar, ast->children[0]->children[i]->data->data);
                }
            }


            ast->children[0]->children[i]->data->data=newvar;
        }
    }
    generate_unique_func_identifier(0,'F');
    fprintf(stdout,"CALL %s\n", ast->data->data);

}
//Generate function definition
void generate_func_def(Nnode ast,HTable *table)
{
    func = true;
    int count = 0;
    generate_unique_func_identifier(0,'F');
    fprintf(stdout, "LABEL %s\n", ast->data->data);
    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@%%RETVAL\n");
    //Rename and generate body of function
    if (ast->children[0]->children[0] != NULL) {
        if (ast->children[0]->data->ntype != BODY) {
            for (int i = 0; ast->children[0]->children[i] != NULL; i++) {
                char *com = malloc(200);
                strcpy(com, ast->children[0]->children[i]->data->data);
                char *newvar = generate_unique_func_identifier(1, 'F');

                ast->children[0]->children[i]->data->data = newvar;
                // rename all params on new name
                ast_rename_value(com, ast->children[1], newvar);
            }
            for (int i = 0; ast->children[1]->children[i] != NULL; i++){
                if (ast->children[1]->children[i]->data->ntype == RETURN) {
                    count++;
                }
            }
            // Generate body of function
            generate(ast->children[1], table);

            if (count == 0) {
                fprintf(stdout, "RETURN\n");
            }
        } else {
            for (int i = 0; ast->children[0]->children[i] != NULL; i++) {
                if (ast->children[0]->children[i]->data->ntype == RETURN) {
                    count++;
                }
            }
            generate(ast->children[0], table);

            if (count == 0) {
                fprintf(stdout, "POPFRAME\n");
                fprintf(stdout, "RETURN\n");
            }
        }

    }
    func = false;
    fprintf(stdout, "\n\n");
}

// Generate return of function
void generate_return(Nnode ast)
{
    fprintf(stdout,"POPFRAME\n");
    if(ast->children[0] != NULL) {
        if (ast->children[0]->data->ntype == VAL) {
            fprintf(stdout, "MOVE TF@%%RETVAL %s\n", ast->children[0]->data->data);
        } else {
            if(ast->children[0]->data->data != NULL){
                fprintf(stdout, "MOVE TF@%%RETVAL TF@%s\n", ast->children[0]->data->data);
            } else {
                fprintf(stdout, "MOVE TF@%%RETVAL nil@nil\n");
            }
        }

    } else
        fprintf(stdout, "MOVE TF@%%RETVAL nil@nil\n");

    fprintf(stdout, "RETURN\n");
}
// Transform name of buildin function into int
int data2int(char *data)
{
    if(!strcmp(data,"inputs"))
    {
        return 0;
    }
    else if(!strcmp(data,"inputi"))
    {
        return 1;
    }
    else if(!strcmp(data,"inputf"))
    {
        return 2;
    }
    else if(!strcmp(data,"print"))
    {
        return 3;
    }
    else if(!strcmp(data,"chr"))
    {
        return 4;
    }
    else if(!strcmp(data,"len"))
    {
        return 5;
    }
    else if(!strcmp(data,"substr"))
    {
        return 6;
    }
    else if(!strcmp(data,"ord"))
    {
        return 7;
    }
    else
        return 8;
}

//Decition which function generate
void indetify_call_function(Nnode ast)
{

    fprintf(stdout, "CREATEFRAME\n");
    int tmp = data2int(ast->data->data);
    switch (tmp){
        case 0:
            fprintf(stdout, "PUSHFRAME\n");
            generate_read_func("string");
            break;
        case 1:

            fprintf(stdout, "PUSHFRAME\n");
            generate_read_func("int");
            break;
        case 2:

            fprintf(stdout, "PUSHFRAME\n");
            generate_read_func("float");
            break;
        case 3:
            generate_write_func(ast);
            return;
            break;
        case 4:
            fprintf(stdout, "DEFVAR TF@%%CHR_1\n");
            if (ast->children[0]->children[0]->data->ntype == VAL) {
                fprintf(stdout, "MOVE TF@%%CHR_1 %s\n",  ast->children[0]->children[0]->data->data);

            } else {

                if (func == true) {
                    fprintf(stdout, "MOVE TF@%%CHR_1 LF@%s\n", ast->children[0]->children[0]->data->data);
                } else {
                    fprintf(stdout, "MOVE TF@%%CHR_1 GF@%s\n", ast->children[0]->children[0]->data->data);
                }

            }

            fprintf(stdout, "CALL $$CHR\n");

            return;
        case 5:
            fprintf(stdout, "DEFVAR TF@F_1\n");
            if (ast->children[0]->children[0]->data->ntype == VAL) {
                fprintf(stdout, "MOVE TF@F_1 %s\n ",  ast->children[0]->children[0]->data->data);

            } else {

                if (func == true) {
                    fprintf(stdout, "MOVE TF@F_1 LF@%s\n", ast->children[0]->children[0]->data->data);
                } else {
                    fprintf(stdout, "MOVE TF@F_1 GF@%s\n", ast->children[0]->children[0]->data->data);
                }

            }
            fprintf(stdout, "CALL $F_LEN\n");
            return;
            break;
        case 6:
            fprintf(stdout, "DEFVAR TF@F_1\n");
            fprintf(stdout, "DEFVAR TF@F_2\n");
            fprintf(stdout, "DEFVAR TF@F_3\n");
            for (int i = 0; i < 3; i++) {
                if (ast->children[0]->children[i]->data->ntype == VAL) {
                    fprintf(stdout, "MOVE TF@F_%d %s\n ", i + 1, ast->children[0]->children[i]->data->data);

                } else {

                    if (func == true) {
                        fprintf(stdout, "MOVE TF@F_%d LF@%s\n", i + 1, ast->children[0]->children[i]->data->data);
                    } else {
                        fprintf(stdout, "MOVE TF@F_%d GF@%s\n", i + 1, ast->children[0]->children[i]->data->data);
                    }

                }
            }
            fprintf(stdout, "CALL F_SUBSTR\n");

            return;
        case 7:
            fprintf(stdout, "DEFVAR TF@F_1\n");
            fprintf(stdout, "DEFVAR TF@F_2\n");
            for (int i = 0; i < 2; i++) {
                if (ast->children[0]->children[i]->data->ntype == VAL) {
                    fprintf(stdout, "MOVE TF@F_%d %s\n ", i + 1, ast->children[0]->children[i]->data->data);

                } else {

                    if (func == true) {
                        fprintf(stdout, "MOVE TF@F_%d LF@%s\n", i + 1, ast->children[0]->children[i]->data->data);
                    } else {
                        fprintf(stdout, "MOVE TF@F_%d GF@%s\n", i + 1, ast->children[0]->children[i]->data->data);
                    }
                }
            }
            fprintf(stdout, "CALL $F_ORD\n");
            return;
            break;
        default:
            generate_call(ast);
            return;
    }


    fprintf(stdout, "POPFRAME\n");
}

// Strlen function
void generate_len_func() {

    fprintf(stdout, "\n");
    fprintf(stdout, "LABEL $F_LEN\n\n");

    fprintf(stdout,"PUSHFRAME\n");

    fprintf(stdout,"DEFVAR LF@%%LEN_TYPE\n");

    fprintf(stdout,"TYPE LF@%%LEN_TYPE LF@F_1\n");
    fprintf(stdout, "JUMPIFEQ ERR_NIL LF@%%LEN_TYPE nil@nil\n");
    fprintf(stdout,"JUMPIFEQ LEN_CONT LF@%%LEN_TYPE string@string\n");
    fprintf(stdout, "LABEL ERR_NIL\n");
    fprintf(stdout,"EXIT int@4\n");
    fprintf(stdout,"LABEL LEN_CONT\n");
    fprintf(stdout,"DEFVAR LF@%%RETVAL\n");
    fprintf(stdout,"STRLEN LF@%%RETVAL LF@F_1\n");
    fprintf(stdout,"POPFRAME\n");
    fprintf(stdout,"RETURN\n\n");

}

// ORD function
void genereate_ord_func() {
    fprintf(stdout, "\n");
    fprintf(stdout, "LABEL $F_ORD\n");

    fprintf(stdout, "PUSHFRAME\n");
    fprintf(stdout, "DEFVAR LF@%%RETVAL\n");
    fprintf(stdout, "DEFVAR LF@ORD_1\n");
    fprintf(stdout, "DEFVAR LF@ORD_CNT\n");
    fprintf(stdout, "DEFVAR LF@ORD_TYPE1\n");
    fprintf(stdout, "DEFVAR LF@ORD_TYPE2\n\n");

    fprintf(stdout, "TYPE LF@ORD_TYPE1 LF@F_1\n");
    fprintf(stdout, "JUMPIFNEQ ORD_ERR LF@ORD_TYPE1 string@string\n");

    fprintf(stdout, "TYPE LF@ORD_TYPE2 LF@F_2\n");
    fprintf(stdout, "JUMPIFEQ ORD_CONT LF@ORD_TYPE2 string@int\n");
    fprintf(stdout, "LABEL ORD_ERR\n");
    fprintf(stdout, "EXIT int@4\n\n");

    fprintf(stdout, "LABEL ORD_CONT\n");
    fprintf(stdout, "CREATEFRAME\n");
    fprintf(stdout, "DEFVAR TF@F_1\n");
    fprintf(stdout, "DEFVAR LF@ORD_BOOL\n");
    fprintf(stdout, "DEFVAR LF@ORD_LEN\n\n");
    fprintf(stdout, "MOVE TF@F_1 LF@F_1\n");

    fprintf(stdout, "CALL $F_LEN\n");
    fprintf(stdout, "MOVE LF@ORD_LEN TF@%%RETVAL\n");
    fprintf(stdout, "SUB LF@ORD_LEN LF@ORD_LEN int@1\n");
    fprintf(stdout, "LT LF@ORD_BOOL LF@F_2 int@0\n");
    fprintf(stdout, "JUMPIFEQ ORD_ERR_OUT_OF_FIELD LF@ORD_BOOL bool@true\n");
    fprintf(stdout, "GT LF@ORD_BOOL LF@F_2 LF@ORD_LEN\n");
    fprintf(stdout, "JUMPIFEQ ORD_ERR_OUT_OF_FIELD LF@ORD_BOOL bool@true\n");
    fprintf(stdout, "JUMP ORD_CONT2\n\n");

    fprintf(stdout, "LABEL ORD_ERR_OUT_OF_FIELD\n");
    fprintf(stdout, "MOVE LF@%%RETVAL nil@nil\n\n");

    fprintf(stdout, "JUMP ORD_END\n");
    fprintf(stdout, "LABEL ORD_CONT2\n");
    fprintf(stdout, "MOVE LF@ORD_1 LF@F_1\n");
    fprintf(stdout, "MOVE LF@ORD_CNT LF@F_2\n");
    fprintf(stdout, "STRI2INT LF@%%RETVAL LF@ORD_1 LF@ORD_CNT\n\n");

    fprintf(stdout, "LABEL ORD_END\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n\n");

}

// SUBSTR function
void generate_substr_func() {

    fprintf(stdout, "\n");
    fprintf(stdout, "LABEL F_SUBSTR\n");
    fprintf(stdout,"PUSHFRAME\n");

    fprintf(stdout, "DEFVAR LF@SBSTR_NOW\n");
    fprintf(stdout, "DEFVAR LF@SBSTR_CNT\n");
    fprintf(stdout, "DEFVAR LF@SBSTR_ITER\n");
    fprintf(stdout, "DEFVAR LF@SBSTR_TYPE\n\n");


    fprintf(stdout, "TYPE LF@SBSTR_TYPE LF@F_1\n");

    fprintf(stdout, "JUMPIFNEQ SBSTR_ERR LF@SBSTR_TYPE string@string\n");

    fprintf(stdout, "TYPE LF@SBSTR_TYPE LF@F_2\n");
    fprintf(stdout, "JUMPIFNEQ SBSTR_ERR LF@SBSTR_TYPE string@int\n");

    fprintf(stdout, "TYPE LF@SBSTR_TYPE LF@F_3\n");
    fprintf(stdout,"JUMPIFNEQ SBSTR_ERR LF@SBSTR_TYPE string@int\n");

    fprintf(stdout, "JUMP SBSTR_CONT\n\n");

    fprintf(stdout, "LABEL SBSTR_ERR\n");
    fprintf(stdout, "EXIT int@4\n\n");

    fprintf(stdout, "LABEL SBSTR_CONT\n");
    fprintf(stdout, "CREATEFRAME\n");
    fprintf(stdout, "DEFVAR LF@SBSTR_BOOL\n");
    fprintf(stdout, "DEFVAR LF@SBSTR_LEN\n");
    fprintf(stdout, "DEFVAR TF@F_1\n");
    fprintf(stdout, "MOVE TF@F_1 LF@F_1\n\n");

    fprintf(stdout, "CALL $F_LEN\n");

    fprintf(stdout, "DEFVAR LF@%%RETVAL\n");
    fprintf(stdout, "MOVE LF@SBSTR_LEN TF@%%RETVAL\n");
    fprintf(stdout, "SUB LF@SBSTR_LEN LF@SBSTR_LEN int@1\n");
    fprintf(stdout, "LT LF@SBSTR_BOOL LF@F_2 int@0\n");
    fprintf(stdout, "JUMPIFEQ SBSTR_OUTOFFIELD LF@SBSTR_BOOL bool@true\n");
    fprintf(stdout, "GT LF@SBSTR_BOOL LF@F_2 LF@SBSTR_LEN\n");
    fprintf(stdout, "JUMPIFEQ SBSTR_OUTOFFIELD LF@SBSTR_BOOL bool@true\n");
    fprintf(stdout, "ADD LF@SBSTR_LEN LF@SBSTR_LEN int@1\n");
    fprintf(stdout, "SUB LF@SBSTR_LEN LF@SBSTR_LEN LF@F_2\n");
    fprintf(stdout, "LT LF@SBSTR_BOOL LF@F_3 int@0\n");
    fprintf(stdout, "JUMPIFEQ SBSTR_OUTOFFIELD LF@SBSTR_BOOL bool@true\n");
    fprintf(stdout, "GT LF@SBSTR_BOOL LF@F_3 LF@SBSTR_LEN\n");
    fprintf(stdout, "MOVE LF@SBSTR_ITER LF@SBSTR_LEN\n");
    fprintf(stdout, "JUMPIFEQ SBSTR_START LF@SBSTR_BOOL bool@true\n");
    fprintf(stdout, "MOVE LF@SBSTR_ITER LF@F_3\n");
    fprintf(stdout, "JUMP SBSTR_START\n\n");

    fprintf(stdout, "LABEL SBSTR_OUTOFFIELD\n");
    fprintf(stdout, "MOVE LF@%%RETVAL nil@nil\n\n");

    fprintf(stdout, "JUMP SBSTR_END\n");
    fprintf(stdout, "LABEL SBSTR_START\n");
    fprintf(stdout, "MOVE LF@%%RETVAL string@\n");
    fprintf(stdout, "MOVE LF@SBSTR_CNT int@0\n");
    fprintf(stdout, "LABEL SBSTR_LOOP\n");
    fprintf(stdout, "JUMPIFEQ SBSTR_END LF@SBSTR_CNT LF@SBSTR_ITER\n");
    fprintf(stdout, "GETCHAR LF@SBSTR_NOW LF@F_1 LF@F_2\n");
    fprintf(stdout, "CONCAT LF@%%RETVAL LF@%%RETVAL LF@SBSTR_NOW\n");
    fprintf(stdout, "ADD LF@SBSTR_CNT LF@SBSTR_CNT int@1\n");
    fprintf(stdout, "ADD LF@F_2 LF@F_2 int@1\n");
    fprintf(stdout, "JUMP SBSTR_LOOP\n\n");
    fprintf(stdout, "LABEL SBSTR_END\n");
    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n\n");
}
// CHr function
void generate_chr_func() {

    fprintf(stdout, "\n");
    fprintf(stdout, "LABEL $$CHR\n");
    fprintf(stdout, "PUSHFRAME\n");

    fprintf(stdout, "DEFVAR LF@%%CHR_TYPE\n");

    fprintf(stdout, "TYPE LF@%%CHR_TYPE LF@%%CHR_1\n");

    fprintf(stdout, "JUMPIFNEQ CHR_QUIT LF@%%CHR_TYPE string@int\n");
    fprintf(stdout, "LT LF@%%CHR_TYPE LF@%%CHR_1 int@0\n");
    fprintf(stdout, "JUMPIFEQ CHR_QUIT LF@%%CHR_TYPE bool@true\n");
    fprintf(stdout, "GT LF@%%CHR_TYPE LF@%%CHR_1 int@255\n");
    fprintf(stdout, "JUMPIFEQ CHR_QUIT LF@%%CHR_TYPE bool@true\n");
    fprintf(stdout, "JUMP CHR_CONT\n");

    fprintf(stdout, "LABEL CHR_QUIT\n");
    fprintf(stdout, "EXIT int@4\n");

    fprintf(stdout, "LABEL CHR_CONT\n");

    fprintf(stdout, "DEFVAR LF@%%RETVAL\n");
    fprintf(stdout, "INT2CHAR LF@%%RETVAL LF@%%CHR_1\n");

    fprintf(stdout, "POPFRAME\n");
    fprintf(stdout, "RETURN\n\n");
}


// Print function
void generate_write_func(Nnode ast) {

    char *tmp = generate_unique_identifier();
    // transform all params of fucntion into local frame
    for(int i = 0; ast->children[0]->children[i]!= NULL; i++)
    {
        fprintf(stdout, "CREATEFRAME\n");
        fprintf(stdout,"DEFVAR TF@%s\n",tmp);
        if (ast->children[0]->children[i]->data->ntype == VAL){
            fprintf(stdout,"MOVE TF@%s %s\n",tmp,ast->children[0]->children[i]->data->data);
        } else if (ast->children[0]->children[i]->data->ntype == VAR){

            if (func == true) {
                fprintf(stdout, "MOVE TF@%s LF@%s\n", tmp, ast->children[0]->children[i]->data->data);
            } else {

                fprintf(stdout, "MOVE TF@%s GF@%s\n", tmp, ast->children[0]->children[i]->data->data);
            }
        }
        fprintf(stdout, "PUSHFRAME\n");
        fprintf(stdout,"WRITE LF@%s\n",tmp);
        fprintf(stdout, "POPFRAME\n");
    }
}


void generate_read_func(char *type) {

    fprintf(stdout, "DEFVAR LF@%%RETVAL\n");
    fprintf(stdout, "READ LF@%%RETVAL %s\n",  type);
}


//Desition which function generate
void identify_header(Nnode ast, HTable *table)
{
    switch (ast->data->ntype) {
        case VAR_DEF:
            generate_vardef(ast);
            break;
        case ASSIGN:

            generate_assign(ast);

            break;
        case IF_NODE:
            generate_if(ast, table);
            break;
        case WHILE:
            generate_while(ast, table);
            break;

        case DIVINIT:
        case DIV:
        case ADD:
        case MUL:
        case SUB:
            generate_math_aritmetic(ast);
            break;
        case COND:
            generate_aritmetic(ast);
            break;
        case CALL:
            indetify_call_function(ast);
            break;
        case FUNC_DEF:
            generate_func_def(ast,table);
            break;
        case RETURN:
            generate_return(ast);
            break;
        default:
            break;
    }
}


// Generate all child of root
int generate(Nnode ast, HTable *table)
{
    if(ast) {
        if (ast->data->ntype == PROG) {
            fflush(stdout);
            create_header();

            type_control();
            type_control_exit();
            generate_len_func();
            generate_substr_func();
            generate_chr_func();
            genereate_ord_func();


            for(int i = 0;ast->children[i] != NULL; i++)
                if(ast->children[i]->data->ntype == FUNC_DEF)
                    identify_header(ast->children[i],table);

            fprintf(stdout, "LABEL $$main\n");
            //Find vardef of variable in while and if  body and vardef variable
            for(int i = 0;ast->children[i] != NULL; i++) {
                if (ast->children[i]->data->ntype == IF_NODE) {
                    for (int k = 0; ast->children[i]->children[1]->children[k] != NULL; k++){
                        if (ast->children[i]->children[1]->children[k]->data->ntype == ASSIGN) {
                            if (ast->children[i]->children[1]->children[k]->children[0]->data->ntype == VAR_DEF) {

                                ast->children[i]->children[1]->children[k]->children[0]->data->inmain = true;
                                fprintf(stdout, "DEFVAR GF@%s\n", ast->children[i]->children[1]->children[k]->children[0]->data->data);
                                ast->children[i]->children[1]->children[k]->children[0]->data->ntype = VAR;
                            }
                        }
                    }
                    if (ast->children[i]->children[2] != NULL)
                    {
                        for (int k = 0; ast->children[i]->children[2]->children[k] != NULL; k++){
                            if (ast->children[i]->children[2]->children[k]->data->ntype == ASSIGN) {
                                if (ast->children[i]->children[2]->children[k]->children[0]->data->ntype == VAR_DEF) {
                                    ast->children[i]->children[2]->children[k]->children[0]->data->inmain = true;
                                    fprintf(stdout, "DEFVAR GF@%s\n", ast->children[i]->children[2]->children[k]->children[0]->data->data);

                                    ast->children[i]->children[2]->children[k]->children[0]->data->ntype = VAR;
                                }
                            }
                        }
                    }

                } else if (ast->children[i]->data->ntype == WHILE){
                    for (int k = 0; ast->children[i]->children[1]->children[k] != NULL; k++){
                        if (ast->children[i]->children[1]->children[k]->data->ntype == ASSIGN) {
                            if (ast->children[i]->children[1]->children[k]->children[0]->data->ntype == VAR_DEF) {

                                ast->children[i]->children[1]->children[k]->children[0]->data->inmain = true;
                                fprintf(stdout, "DEFVAR GF@%s\n", ast->children[i]->children[1]->children[k]->children[0]->data->data);
                                ast->children[i]->children[1]->children[k]->children[0]->data->ntype = VAR;
                            }
                        }
                    }
                }
            }
            create_local_frame();

        } // generate body
        if (ast->data->ntype == PROG || ast->data->ntype == BODY || ast->data->ntype == IF_BODY || ast->data->ntype == IF_ELSE) {
            for (int i = 0; i < ast->data->child_count; i++) {
                generate(ast->children[i], table);
            }
        } else if (ast->data->ntype != PROG && ast->data->ntype != FUNC_DEF) {
            identify_header(ast, table);
        }
        if (ast->data->ntype == PROG) {
            end_frame();
        }

    }
    return 0;
}
