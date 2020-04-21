CXX = g++
CFLAGS= -g -c -Wall -Wextra -Wwrite-strings
LFLAGS= -g
prog4: prog4_funcs.o prog4.o Count.o
	${CXX} ${LFLAGS} -o prog4 prog4_funcs.o prog4.o Count.o
Count.o: Count.cpp Count.h
	${CXX} ${CFLAGS} Count.cpp
prog4_funcs.o: prog4_funcs.cpp prog4_funcs.h
	${CXX} ${CFLAGS} prog4_funcs.cpp
prog4.o: prog4_funcs.cpp Count.h prog4_funcs.h
	${CXX} ${CFLAGS} prog4.cpp
clean:
	rm -f *.o
	rm -f prog4
