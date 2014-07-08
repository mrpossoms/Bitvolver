FLAGS=-Wint-to-pointer-cast
SRC=./src/*.c
INC=./include
DST=./lib
LINK=-lpthread
NAME=bitvolver

TST=./tests
TSTS=./tests/*.c
TSTINC=-I$(TST)/include
TSTLIB=-L$(TST)/lib

all:
	mkdir -p lib
	gcc -I$(INC) -c $(SRC)
	ar rcs $(DST)/libbitvolver.a *.o
	rm *.o
testlib: $(SRC)
	gcc $(TSTINC) -c $(SRC)
	ar rcs $(TST)/lib/lib$(NAME).a *.o
	rm *.o

tests: testlib
	mkdir -p $(TST)/bin
	find $(TST) -name "*.c" -exec gcc $(TSTINC) $(TSTLIB) {} -o {}.bin -l$(NAME) $(LINK) \;
clean:
	find ./ -iname "*.a" -exec rm {} \;
	find ./ -iname "*.o" -exec rm {} \;
	find ./ -iname "*.out" -exec rm {} \;
	find ./ -iname "*.bin" -exec rm {} \;