#!/bin/bash

build/debug/bin/dlangparser --dump-asm $1
llc intermediate.ll
clang -c intermediate.s
clang intermediate.o -o $2