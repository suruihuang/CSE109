CC=gcc
CFLAGS=-g -Wall -Wextra -Wwrite-strings -DDEBUG
LFLAGS=-lcurl -g

prog3: prog3.o prog3_funcs.o
	${CC} ${LFLAGS} -o prog3 prog3.o prog3_funcs.o
prog3.o: prog3.c
	${CC} ${CFLAGS} -c prog3.c
prog3_funcs.o: prog1_funcs.c
	${CC} ${CFLAGS} -c prog3_funcs.c
clean: 
	rm -f *.o
	rm -f prog3
