# Copyright 2014 James Sinclair
# This file is part of Array-BPXOR Secret-sharer.
#
#    Array-BPXOR Secret-sharer is free software: you can redistribute it and/or
#    modify it under the terms of the GNU General Public License as published
#    by the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    Array-BPXOR Secret-sharer is distributed in the hope that it will be
#    useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with Array-BPXOR Secret-sharer.  If not, 
#    see <http://www.gnu.org/licenses/>.
#
#--------------------------------------------------------------------------
CC=g++
CFLAGS=-Wall
INCLUDES=-I./headers/codec -I/opt/local/include
LIBDIR=-L/opt/local/lib
LDFLAGS=-lboost_system -lboost_filesystem
TEST1=./src/test/testcases_bnmatrix.cpp
TEST2=./src/test/testcases_sharer.cpp
MAIN=./src/secretsharer.cpp
LIB1=./src/codec/restrictedbnmatrix.cpp
LIB2=./src/codec/blocksecretsharer.cpp
BIN=/usr/bin/
TEST1EXE=test_restrictedbnmatrix
TEST2EXE=test_secretsharer
EXE=secretsharer
KEYFILE=~/.sskey
#--------------------------------------------------------------------------

all: cleanest $(EXE) $(TEST1EXE) $(TEST2EXE)
main: cleanest $(EXE)
tests: cleanest $(TEST1EXE) $(TEST2EXE)


$(EXE): $(MAIN) $(LIB1) $(LIB2)
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBDIR) $? -o $@ $(LDFLAGS)

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

purge: cleanest
	rm -f $(KEYFILE)

install: main
	mv $(EXE) $(BIN)

uninstall: purge
	rm -f $(BIN)$(EXE)
