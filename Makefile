CC=gcc
CFLAGS= -std=c99 -Wall -Wextra -g
TESTER=tests/tester.py

ifj19: parser.c parser.h scanner.c scanner.h ast.c ast.h generator.c generator.h functions.c functions.h list.c list.h stack.c stack.h symtable.c symtable.h posfix_func.c posfix_func.h
	$(CC) $(CFLAGS) parser.c -o ifj19

test: ifj19
	$(CC) $(CFLAGS) parser.c -o ifj19
	python3 $(TESTER)
