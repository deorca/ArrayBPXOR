#--------------------------------------------------------------------------
CC=g++
CFLAGS=-I./src -I./test -g
TEST1=testcases_bnmatrix.cpp
TEST2=testcases_sharer.cpp
MAIN=secretsharer.cpp
LIB1=restrictedbnmatrix.cpp
LIB2=blocksecretsharer.cpp
TEST1EXE=test_restrictedbnmatrix
TEST2EXE=test_secretsharer
EXE=secretsharer
#--------------------------------------------------------------------------

all: cleanest $(EXE) $(TEST1EXE) $(TEST2EXE)
main: cleanest $(EXE)
tests: cleanest $(TEST1EXE) $(TEST2EXE)


$(EXE): $(MAIN) $(LIB1) $(LIB2)
	$(CC) $(CFLAGS) $? -o $@

$(TEST1EXE): $(TEST1) $(LIB1) $(LIB2)
	$(CC) $(CFLAGS) $? -o $@

$(TEST2EXE): $(TEST2) $(LIB1) $(LIB2)
	$(CC) $(CFLAGS) $? -o $@

.PHONY: clean cleanest

clean:
	rm -f *.o *.gch
	rm -rf *.dSYM

cleanest: clean
	rm -f $(EXE)

