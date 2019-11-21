cmake_minimum_required(VERSION 3.14)
project(scanner.c)

set(CMAKE_C_STANDARD 99)

add_executable(ifj parser.c parser.h list.c list.h scanner.c scanner.h errors.h list.c list.h symtable.c symtable.h functions.c functions.h stack.c stack.h generator.c generator.h myast.h myast.c)
