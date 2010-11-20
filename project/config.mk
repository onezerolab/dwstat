COMPILER = gcc
CLEAN = rm -rf
STRIP = strip

OUTPUT = dwstat
OUT_DIR = .
LIBS = X11

DEBUG_FLAGS = -Wall -pedantic-errors

RELEASE_FLAGS = -Wall -pedantic-errors -O3 -DNDEBUG

ROOT = ..
