#!/bin/bash

# GPMF library

LFMakeMaker -v +f=GPMFMakeFile -cc="gcc -Wall -O2" gpmf-parser/*.c gpmf-parser/demo/GPMF_mp4reader.c -ar=libGPMF.a > GPMFMakeFile

# rebuild data utility library
cd datalib

LFMakeMaker -v +f=Makefile -cc="g++ -Wall -O2" --opts="-lm" *.cpp -ar=../libdatalib.a > Makefile 

cd ..

# rebuild data utility library
cd gfxlib

LFMakeMaker -v +f=Makefile -cc="g++ -Wall -O2" --opts="-lm \$(shell pkg-config --cflags cairo ) \$(shell pkg-config --libs cairo )" *.cpp -ar=../libgfxlib.a > Makefile 

cd ..

# Final executables

LFMakeMaker -v +f=MakefileLocal +f=libdatalib.a +f=libgfxlib.a -cc="g++ -Wall -O2" --opts="-lm -L. -lgfxlib -ldatalib -lGPMF \$(shell pkg-config --cflags cairo ) \$(shell pkg-config --libs cairo )" GPMFMetersGenerator.cpp -t=GPMFMetersGenerator mkStory.cpp -t=mkStory > MakefileLocal
