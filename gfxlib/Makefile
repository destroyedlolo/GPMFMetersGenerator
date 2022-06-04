# makefile created automaticaly by LFMakeMaker
# LFMakeMaker 1.6 (May  7 2022 20:46:23) (c)LFSoft 1997

gotoall: all


#The compiler (may be customized for compiler's options).
cc=g++ -Wall -O2
opts=-lm $(shell pkg-config --cflags cairo ) $(shell pkg-config --libs cairo )

AltitudeGfx.o : AltitudeGfx.cpp AltitudeGfx.h Gfx.h \
  ../datalib/Context.h ../datalib/GPVideo.h ../datalib/GPX.h Makefile 
	$(cc) -c -o AltitudeGfx.o AltitudeGfx.cpp $(opts) 

Export.o : Export.cpp Export.h ../datalib/Context.h \
  ../datalib/GPVideo.h Makefile 
	$(cc) -c -o Export.o Export.cpp $(opts) 

Gfx.o : Gfx.cpp Gfx.h ../datalib/Context.h ../datalib/GPVideo.h \
  ../datalib/GPX.h Makefile 
	$(cc) -c -o Gfx.o Gfx.cpp $(opts) 

PathGfx.o : PathGfx.cpp PathGfx.h Gfx.h ../datalib/Context.h \
  ../datalib/GPVideo.h ../datalib/GPX.h Makefile 
	$(cc) -c -o PathGfx.o PathGfx.cpp $(opts) 

QualityGfx.o : QualityGfx.cpp QualityGfx.h Gfx.h ../datalib/Context.h \
  ../datalib/GPVideo.h ../datalib/GPX.h Makefile 
	$(cc) -c -o QualityGfx.o QualityGfx.cpp $(opts) 

SpeedGfx.o : SpeedGfx.cpp SpeedGfx.h Gfx.h ../datalib/Context.h \
  ../datalib/GPVideo.h ../datalib/GPX.h Makefile 
	$(cc) -c -o SpeedGfx.o SpeedGfx.cpp $(opts) 

SpeedTrkGfx.o : SpeedTrkGfx.cpp SpeedTrkGfx.h Gfx.h \
  ../datalib/Context.h ../datalib/GPVideo.h ../datalib/GPX.h Makefile 
	$(cc) -c -o SpeedTrkGfx.o SpeedTrkGfx.cpp $(opts) 

TrkStatGfx.o : TrkStatGfx.cpp TrkStatGfx.h Makefile 
	$(cc) -c -o TrkStatGfx.o TrkStatGfx.cpp $(opts) 

../libgfxlib.a : TrkStatGfx.o SpeedTrkGfx.o SpeedGfx.o QualityGfx.o \
  PathGfx.o Gfx.o Export.o AltitudeGfx.o Makefile 
	 ar rcs  ../libgfxlib.a TrkStatGfx.o SpeedTrkGfx.o SpeedGfx.o \
  QualityGfx.o PathGfx.o Gfx.o Export.o AltitudeGfx.o 

all: ../libgfxlib.a 
