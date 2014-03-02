#--------------------------------------------------------------------------
CC=g++
CFLAGS=-Wall -g
INCLUDES=-I./headers/codec
LDFLAGS=-lboost_system -lboost_filesystem
TEST1=./src/test/testcases_bnmatrix.cpp
TEST2=./src/test/testcases_sharer.cpp
MAIN=./src/secretsharer.cpp
LIB1=./src/codec/restrictedbnmatrix.cpp
LIB2=./src/codec/blocksecretsharer.cpp
TEST1EXE=test_restrictedbnmatrix
TEST2EXE=test_secretsharer
EXE=secretsharer
#--------------------------------------------------------------------------

all: cleanest $(EXE) $(TEST1EXE) $(TEST2EXE)
main: cleanest $(EXE)
#tests: cleanest $(TEST1EXE) $(TEST2EXE)
tests: cleanest $(TEST2EXE)


$(EXE): $(MAIN) $(LIB1) $(LIB2)
	$(CC) $(CFLAGS) $(INCLUDES) $? -o $@ $(LDFLAGS)

$(TEST1EXE): $(TEST1) $(LIB1) $(LIB2)
	$(CC) $(CFLAGS) $? -o $@

$(TEST2EXE): $(TEST2) $(LIB1) $(LIB2)
	$(CC) $(CFLAGS) $? -o $@

.PHONY: clean cleanest

clean:
	rm -f *.o *.gch
	rm -rf *.dSYM

cleanest: clean
	rm -f $(EXE) $(TEST1EXE) $(TEST2EXE)

