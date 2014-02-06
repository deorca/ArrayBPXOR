#--------------------------------------------------------------------------
CC=g++
CFLAGS=-I./src -I./test -g
MAIN=testcases_sharer.cpp
LIB1=restrictedbnmatrix.cpp
LIB2=blocksecretsharer.cpp
#EXE=ArrayBpXorEncoding
EXE=TestArrayBpXorEncoding
#--------------------------------------------------------------------------

all: cleanest $(EXE)

$(EXE): $(MAIN) $(LIB1) $(LIB2)
	$(CC) $(CFLAGS) $? -o $@

.PHONY: clean cleanest

clean:
	rm -f *.o *.gch

cleanest: clean
	rm -f $(EXE)

