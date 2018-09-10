LIB=./lib
INCLUDE=./include
SRC=./src
OBJ=./obj
FLAGS=  -O3 -Wall

libMO445: $(LIB)/libMO445.a
	echo "libMO445.a built..."

$(LIB)/libMO445.a: \
$(OBJ)/MO445.o \

	ar csr $(LIB)/libMO445.a \
$(OBJ)/MO445.o \

$(OBJ)/MO445.o: $(SRC)/MO445.c
	gcc $(FLAGS) -c $(SRC)/MO445.c -I$(INCLUDE) \
	-o $(OBJ)/MO445.o

clean: 
	rm $(LIB)/lib*.a; rm $(OBJ)/*.o





