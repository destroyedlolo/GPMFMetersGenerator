# makefile created automaticaly by LFMakeMaker
# LFMakeMaker 1.6 (May  9 2022 15:11:43) (c)LFSoft 1997

gotoall: all


#The compiler (may be customized for compiler's options).
cc=g++ -Wall -O2
opts=-lm $(shell pkg-config --cflags cairo ) $(shell pkg-config --libs cairo )

AltitudeGfx.o : AltitudeGfx.cpp AltitudeGfx.h Gfx.h \
  ../datalib/Context.h ../datalib/GPVideo.h Makefile 
	$(cc) -c -o AltitudeGfx.o AltitudeGfx.cpp $(opts) 

Gfx.o : Gfx.cpp Gfx.h ../datalib/Context.h ../datalib/GPVideo.h \
  Makefile 
	$(cc) -c -o Gfx.o Gfx.cpp $(opts) 

SpeedGfx.o : SpeedGfx.cpp SpeedGfx.h Gfx.h ../datalib/Context.h \
  ../datalib/GPVideo.h Makefile 
	$(cc) -c -o SpeedGfx.o SpeedGfx.cpp $(opts) 

../libgfxlib.a : SpeedGfx.o Gfx.o AltitudeGfx.o Makefile 
	 ar rcs  ../libgfxlib.a SpeedGfx.o Gfx.o AltitudeGfx.o 

all: ../libgfxlib.a 
