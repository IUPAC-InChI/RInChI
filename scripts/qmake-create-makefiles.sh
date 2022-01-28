#!/bin/bash

qmake -o Makefile-64bit $1
qmake -o Makefile-32bit -spec linux-g++-32 $1
