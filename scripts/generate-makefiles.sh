#!/bin/bash

SCRIPT_DIR="$(dirname -- "${BASH_SOURCE[0]}")"

qmake -o Makefile-64bit $1
qmake -o Makefile-32bit -spec linux-g++-32 $1

python $SCRIPT_DIR/cleanup_qmake_makefiles.py Makefile-64bit $1 > Makefile-64bit.new
mv Makefile-64bit.new Makefile-64bit

python $SCRIPT_DIR/cleanup_qmake_makefiles.py Makefile-32bit $1 > Makefile-32bit.new
mv Makefile-32bit.new Makefile-32bit

# The Mac makefile for building on an M1 cloud instance is identical to the
# Makefile-64bit, except that we have to remove the "-Wl,-O1" option from the
# linker flags.
cat Makefile-64bit | sed 's/LFLAGS        = -m64 -Wl,-O1/LFLAGS        = -m64/' | sed 's/Makefile-64bit/Makefile-Mac/' > Makefile-Mac

# The Raspberry makefile is identical to the Makefile-64bit, except that the
# "-m64" flag is removed.
cat Makefile-64bit | sed 's/-m64//' | sed 's/Makefile-64bit/Makefile-Raspberry/' > Makefile-Raspberry

