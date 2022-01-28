# makefile created automaticaly by LFMakeMaker
# LFMakeMaker 1.4 (Apr  3 2018 23:14:49) (c)LFSoft 1997

gotoall: all


#The compiler (may be customized for compiler's options).
cc=gcc -Wall -O2
opts=

GPMFMetersGenerator.o : GPMFMetersGenerator.c \
  gpmf-parser/GPMF_parser.h gpmf-parser/GPMF_utils.h \
  gpmf-parser/demo/GPMF_mp4reader.h Makefile 
	$(cc) -c -o GPMFMetersGenerator.o GPMFMetersGenerator.c \
  $(opts) 

gpmf-parser/GPMF_parser.o : gpmf-parser/GPMF_parser.c Makefile 
	$(cc) -c -o gpmf-parser/GPMF_parser.o \
  gpmf-parser/GPMF_parser.c $(opts) 

gpmf-parser/GPMF_utils.o : gpmf-parser/GPMF_utils.c Makefile 
	$(cc) -c -o gpmf-parser/GPMF_utils.o gpmf-parser/GPMF_utils.c \
  $(opts) 

gpmf-parser/demo/GPMF_mp4reader.o : gpmf-parser/demo/GPMF_mp4reader.c \
  Makefile 
	$(cc) -c -o gpmf-parser/demo/GPMF_mp4reader.o \
  gpmf-parser/demo/GPMF_mp4reader.c $(opts) 

GPMFMetersGenerator : gpmf-parser/demo/GPMF_mp4reader.o \
  gpmf-parser/GPMF_utils.o gpmf-parser/GPMF_parser.o \
  GPMFMetersGenerator.o Makefile 
	 $(cc) -o GPMFMetersGenerator \
  gpmf-parser/demo/GPMF_mp4reader.o gpmf-parser/GPMF_utils.o \
  gpmf-parser/GPMF_parser.o GPMFMetersGenerator.o $(opts) 

all: GPMFMetersGenerator 
