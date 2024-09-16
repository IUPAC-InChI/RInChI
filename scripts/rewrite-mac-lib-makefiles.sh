#!/bin/bash

# Mac shared libraries should have a .dylib extension
# instead of .so.
sed -i '' -e '/^TARGET/ s/\.so/\.dylib/' Makefile-Mac

# For Macs on ARM with an x86 Python you will want to
# build the shared library for x86 so it can be loaded
# from Python.
cat Makefile-Mac | sed 's/^CFLAGS        =/CFLAGS        = -arch x86_64/' | sed 's/^CXXFLAGS      =/CXXFLAGS      = -arch x86_64/' | sed 's/^LFLAGS        =/LFLAGS        = -arch x86_64/' > Makefile-Mac-arm64-with-x86-Python
