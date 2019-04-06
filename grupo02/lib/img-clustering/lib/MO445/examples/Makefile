CFLAGS = -Wall -O3
MO445LIB  = -L../lib
MO445FLAGS  = -I../include
CC=gcc
FLAGS= -O3 -Wall

all: libMO445 test

libMO445:
	$(MAKE) -C ../
test: libMO445 
	$(CC) $(FLAGS) $(MO445FLAGS) test.c  $(MO445LIB) -o test -lMO445 -lm
clean:
	rm -f *~ test.o test ../lib/*.a ../obj/*.o
