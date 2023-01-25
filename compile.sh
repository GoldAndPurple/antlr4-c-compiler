#!/bin/bash

if [ $# -ne 2 ] 
then echo "Usage: ./compile.sh <input source code filename> <output executable filename>"
exit -1
fi

build/bin/dlangparser $1
llc intermediate.ll
clang -c intermediate.s
clang intermediate.o -o $2