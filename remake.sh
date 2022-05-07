#!/bin/bash

# rebuild utility library
cd datalib
LFMakeMaker -v +f=Makefile -cc="gcc -Wall -O2" --opts="-lm \$(shell pkg-config --cflags cairo ) \$(shell pkg-config --libs cairo )" *.c ../gpmf-parser/*.c ../gpmf-parser/demo/GPMF_mp4reader.c -ar=lib_datalib.a > Makefile 

cd ..

# Recreate the makefile

LFMakeMaker -v +f=Makefile -cc="gcc -Wall -O2" --opts="-lm -L./datalib/ -l_datalib \$(shell pkg-config --cflags cairo ) \$(shell pkg-config --libs cairo )" GPMFMetersGenerator.c -t=GPMFMetersGenerator > Makefile

echo '	$(MAKE) -C datalib' >> Makefile

