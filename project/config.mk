COMPILER = gcc
CLEAN = rm -rf
STRIP = strip

OUTPUT = dstat
OUT_DIR = .
LIBS = X11

DEBUG_FLAGS = -Wall -pedantic-errors

RELEASE_FLAGS = -Wall -pedantic-errors -O3 -DNDEBUG

ROOT = ..
