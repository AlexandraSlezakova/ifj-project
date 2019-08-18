#include "symtable.h"

int recursive_descent();
HTable *recursive_descent_id();
int recursive_descent_defFn();
int recursive_descent_rvalue(bool assignment);
int recursive_descent_formParams();
int recursive_descent_nextFormParam();
int recursive_descent_realParams();
int recursive_descent_nextRealParam();
int recursive_descent_if();
int recursive_descent_while();
struct TToken *previousTokenMain;
struct TToken *tmpTokenMain;
//struct TToken *previousToken;
struct TToken *tmpToken;
static int counter = 0;
static int bracket_counter = 0;
char* actualFunctionName;