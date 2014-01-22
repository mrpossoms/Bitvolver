FLAGS=-Wint-to-pointer-cast
SRC=./src/*.c
INC=./include
DST=./lib
LINK=-pthread -string
LIB=libbitvolver.a

TSTINC=./tests/include
TST=./tests
TSTS=./tests/*.c

all:
	gcc -I$(INC) -c $(SRC)
	ar rcs $(DST)/libbitvolver.a *.o
	rm *.o
testlib: $(SRC)
	gcc -I$(TSTINC) -c $(SRC)
	ar rcs $(TST)/lib/$(LIB) *.o
	rm *.o

tests: testlib
	$(foreach test, $(TSTS), gcc $(test) -o $(TST)/bin/$(test).bin
clean:
	find ./ -iname "*.a" -exec rm {} \;
	find ./ -iname "*.o" -exec rm {} \;
	find ./ -iname "*.out" -exec rm {} \;
