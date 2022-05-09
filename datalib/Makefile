# makefile created automaticaly by LFMakeMaker
# LFMakeMaker 1.6 (May  9 2022 10:57:45) (c)LFSoft 1997

gotoall: all


#The compiler (may be customized for compiler's options).
cc=g++ -Wall -O2
opts=-lm $(shell pkg-config --cflags cairo ) $(shell pkg-config --libs cairo )

Context.o : Context.cpp Context.h Makefile 
	$(cc) -c -o Context.o Context.cpp $(opts) 

../libdatalib.a : Context.o Makefile 
	 ar rcs  ../libdatalib.a Context.o 

all: ../libdatalib.a 
