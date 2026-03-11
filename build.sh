#!/bin/bash
set -e

mkdir -p bin

gcc $PROGRAM_COMPILER_FLAGS -Iinclude src/taskbar.c -o bin/taskbar.o
gcc $PROGRAM_COMPILER_FLAGS -Iinclude src/clock.c -o bin/clock.o
gcc $PROGRAM_COMPILER_FLAGS -Iinclude src/drawing.c -o bin/drawing.o
gcc $PROGRAM_COMPILER_FLAGS -Iinclude src/inputs.c -o bin/inputs.o
gcc $PROGRAM_COMPILER_FLAGS -Iinclude src/windows.c -o bin/windows.o
gcc $PROGRAM_COMPILER_FLAGS -Iinclude src/desktop.c -o bin/desktop.o

ld -r bin/taskbar.o bin/clock.o bin/drawing.o bin/inputs.o bin/windows.o bin/desktop.o -o bin/final.o

nm bin/final.o > bin/desktop.sym

ld $PROGRAM_LINKER_FLAGS bin/final.o -o bin/desktop.nxe
