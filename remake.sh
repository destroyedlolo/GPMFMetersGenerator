#!/bin/bash

# GPMF library

LFMakeMaker -v +f=GPMFMakeFile -cc="gcc -Wall -O2" gpmf-parser/*.c gpmf-parser/demo/GPMF_mp4reader.c -ar=libGPMF.a > GPMFMakeFile


LFMakeMaker -v +f=MakefileLocal -cc="g++ -Wall -O2" --opts="-lm \$(shell pkg-config --cflags cairo ) \$(shell pkg-config --libs cairo )" GPMFMetersGenerator.cpp -t=GPMFMetersGenerator > MakefileLocal
