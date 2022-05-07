# makefile created automaticaly by LFMakeMaker
# LFMakeMaker 1.6 (May  7 2022 20:46:23) (c)LFSoft 1997

gotoall: all


#The compiler (may be customized for compiler's options).
cc=gcc -Wall -O2
opts=-lm $(shell pkg-config --cflags cairo ) $(shell pkg-config --libs cairo )

GPMFdata.o : GPMFdata.c Shared.h GPMFdata.h Makefile 
	$(cc) -c -o GPMFdata.o GPMFdata.c $(opts) 

GpxHelper.o : GpxHelper.c Shared.h GpxHelper.h Makefile 
	$(cc) -c -o GpxHelper.o GpxHelper.c $(opts) 

Shared.o : Shared.c Shared.h Makefile 
	$(cc) -c -o Shared.o Shared.c $(opts) 

../gpmf-parser/GPMF_parser.o : ../gpmf-parser/GPMF_parser.c Makefile 
	$(cc) -c -o ../gpmf-parser/GPMF_parser.o \
  ../gpmf-parser/GPMF_parser.c $(opts) 

../gpmf-parser/GPMF_utils.o : ../gpmf-parser/GPMF_utils.c Makefile 
	$(cc) -c -o ../gpmf-parser/GPMF_utils.o \
  ../gpmf-parser/GPMF_utils.c $(opts) 

../gpmf-parser/demo/GPMF_mp4reader.o : \
  ../gpmf-parser/demo/GPMF_mp4reader.c Makefile 
	$(cc) -c -o ../gpmf-parser/demo/GPMF_mp4reader.o \
  ../gpmf-parser/demo/GPMF_mp4reader.c $(opts) 

lib_datalib.a : ../gpmf-parser/demo/GPMF_mp4reader.o \
  ../gpmf-parser/GPMF_utils.o ../gpmf-parser/GPMF_parser.o Shared.o \
  GpxHelper.o GPMFdata.o Makefile 
	 ar rcs  lib_datalib.a ../gpmf-parser/demo/GPMF_mp4reader.o \
  ../gpmf-parser/GPMF_utils.o ../gpmf-parser/GPMF_parser.o Shared.o \
  GpxHelper.o GPMFdata.o 

all: lib_datalib.a 
