# makefile created automaticaly by LFMakeMaker
# LFMakeMaker 1.6 (May  7 2022 20:46:23) (c)LFSoft 1997

gotoall: all


#The compiler (may be customized for compiler's options).
cc=g++ -Wall -O2
opts=-lm $(shell pkg-config --cflags cairo ) $(shell pkg-config --libs cairo )

Context.o : Context.cpp Context.h Makefile 
	$(cc) -c -o Context.o Context.cpp $(opts) 

GPVideo.o : GPVideo.cpp ../gpmf-parser/GPMF_parser.h \
  ../gpmf-parser/GPMF_utils.h ../gpmf-parser/demo/GPMF_mp4reader.h \
  Context.h GPVideo.h Makefile 
	$(cc) -c -o GPVideo.o GPVideo.cpp $(opts) 

../libdatalib.a : GPVideo.o Context.o Makefile 
	 ar rcs  ../libdatalib.a GPVideo.o Context.o 

all: ../libdatalib.a 
