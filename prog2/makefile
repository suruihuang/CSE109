CC=gcc
CFLAGS=-g -Wall -Wextra -Wwrite-strings
LFLAGS=-g 

prog2: prog2.o prog2_funcs.o
	${CC} ${LFLAGS} -o prog2 prog2.o prog2_funcs.o
prog2.o: prog2.c
	${CC} ${CFLAGS} -c prog2.c 
prog2_funcs.o: prog2_funcs.c
	${CC} ${CFLAGS} -c prog2_funcs.c
clean: 
	rm -f *.o
	rm -f prog2
