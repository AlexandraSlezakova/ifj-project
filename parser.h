#include "semantic.h"

struct TToken *previousToken;
struct TToken *tmpToken;
static int counter = 0;
int get_next_token();
tDLList *functionParams(tDLList *function_param_list, HTable *local_symtable);
HTable *recursive_descent_id(int hashtable_switch, HTable *local_symtable);
int function_definition();
