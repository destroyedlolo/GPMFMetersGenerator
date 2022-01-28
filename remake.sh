#!/bin/bash

# Recreate the makefile

LFMakeMaker -v +f=Makefile -cc="gcc -Wall -O2" *.c gpmf-parser/*.c gpmf-parser/demo/GPMF_mp4reader.c -t=GPMFMetersGenerator > Makefile
