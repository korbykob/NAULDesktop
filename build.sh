#!/bin/bash
set -e

mkdir -p bin

COMPILER_FLAGS="$PROGRAM_COMPILER_FLAGS -Iinclude -g -fno-omit-frame-pointer -mavx2 -O2 -fvect-cost-model=dynamic -Wall -Wextra -Werror"

gcc $COMPILER_FLAGS src/taskbar.c -o bin/taskbar.o
gcc $COMPILER_FLAGS src/clock.c -o bin/clock.o
gcc $COMPILER_FLAGS src/drawing.c -o bin/drawing.o
gcc $COMPILER_FLAGS src/inputs.c -o bin/inputs.o
gcc $COMPILER_FLAGS src/windows.c -o bin/windows.o
gcc $COMPILER_FLAGS src/desktop.c -o bin/desktop.o

ld -r bin/taskbar.o bin/clock.o bin/drawing.o bin/inputs.o bin/windows.o bin/desktop.o -o bin/final.o

nm bin/final.o > bin/desktop.sym

ld $PROGRAM_LINKER_FLAGS bin/final.o -o bin/desktop.nxe
