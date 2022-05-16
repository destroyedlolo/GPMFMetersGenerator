# makefile created automaticaly by LFMakeMaker
# LFMakeMaker 1.6 (May  9 2022 15:11:43) (c)LFSoft 1997

gotoall: all


#The compiler (may be customized for compiler's options).
cc=g++ -Wall -O2
opts=-lm

Context.o : Context.cpp Context.h Makefile 
	$(cc) -c -o Context.o Context.cpp $(opts) 

GPVideo.o : GPVideo.cpp ../gpmf-parser/GPMF_parser.h \
  ../gpmf-parser/GPMF_utils.h ../gpmf-parser/demo/GPMF_mp4reader.h \
  Context.h GPVideo.h Makefile 
	$(cc) -c -o GPVideo.o GPVideo.cpp $(opts) 

GPX.o : GPX.cpp ../datalib/Context.h GPX.h Makefile 
	$(cc) -c -o GPX.o GPX.cpp $(opts) 

../libdatalib.a : GPX.o GPVideo.o Context.o Makefile 
	 ar rcs  ../libdatalib.a GPX.o GPVideo.o Context.o 

all: ../libdatalib.a 
