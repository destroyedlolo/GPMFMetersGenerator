#!/bin/bash

# rebuild data utility library
cd datalib

LFMakeMaker -v +f=Makefile -cc="gcc -Wall -O2" --opts="-lm \$(shell pkg-config --cflags cairo ) \$(shell pkg-config --libs cairo )" *.c ../gpmf-parser/*.c ../gpmf-parser/demo/GPMF_mp4reader.c -ar=lib_datalib.a > Makefile 

cd ..

# rebuild graphics generation library

cd gfxlib

LFMakeMaker -v +f=Makefile -cc="gcc -Wall -O2" --opts="-lm \$(shell pkg-config --cflags cairo ) \$(shell pkg-config --libs cairo )" *.c -ar=lib_gfxgen.a > Makefile 

cd ..

# Recreate the makefile

LFMakeMaker -v +f=MakefileLocal -cc="gcc -Wall -O2" --opts="-lm -L./datalib/ -l_datalib -L./gfxlib/ -l_gfxgen \$(shell pkg-config --cflags cairo ) \$(shell pkg-config --libs cairo )" GPMFMetersGenerator.c -t=GPMFMetersGenerator > MakefileLocal

