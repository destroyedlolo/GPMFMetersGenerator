#!/bin/bash

# GPMF library

LFMakeMaker -v +f=GPMFMakeFile -cc="gcc -Wall -O2" gpmf-parser/*.c gpmf-parser/demo/GPMF_mp4reader.c -ar=libGPMF.a > GPMFMakeFile

# rebuild data utility library
cd datalib

LFMakeMaker -v +f=Makefile -cc="g++ -Wall -O2" --opts="-lm \$(shell pkg-config --cflags cairo ) \$(shell pkg-config --libs cairo )" *.cpp -ar=../libdatalib.a > Makefile 

cd ..

# Final executables

LFMakeMaker -v +f=MakefileLocal -cc="g++ -Wall -O2" --opts="-lm -L. -ldatalib \$(shell pkg-config --cflags cairo ) \$(shell pkg-config --libs cairo )" GPMFMetersGenerator.cpp -t=GPMFMetersGenerator > MakefileLocal
