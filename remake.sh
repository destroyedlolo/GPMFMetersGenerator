#!/bin/bash

# Recreate the makefile

LFMakeMaker -v +f=Makefile -cc="g++ -Wall -O2" --opts="-lm \$(shell pkg-config --cflags cairo ) \$(shell pkg-config --libs cairo )" GPMFMetersGenerator.cpp -t=GPMFMetersGenerator > MakefileLocal
