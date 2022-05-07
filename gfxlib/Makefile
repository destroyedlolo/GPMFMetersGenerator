# makefile created automaticaly by LFMakeMaker
# LFMakeMaker 1.6 (May  7 2022 20:46:23) (c)LFSoft 1997

gotoall: all


#The compiler (may be customized for compiler's options).
cc=gcc -Wall -O2
opts=-lm $(shell pkg-config --cflags cairo ) $(shell pkg-config --libs cairo )

AltitudeGraphic.o : AltitudeGraphic.c Shared.h ../datalib/Shared.h \
  AltitudeGraphic.h ../datalib/GPMFdata.h ../datalib/GpxHelper.h \
  Makefile 
	$(cc) -c -o AltitudeGraphic.o AltitudeGraphic.c $(opts) 

PathGraphic.o : PathGraphic.c Shared.h ../datalib/Shared.h \
  ../datalib/GPMFdata.h ../datalib/GpxHelper.h PathGraphic.h Makefile 
	$(cc) -c -o PathGraphic.o PathGraphic.c $(opts) 

SpeedGraphic.o : SpeedGraphic.c SpeedGraphic.h ../datalib/GPMFdata.h \
  ../datalib/Shared.h Shared.h Makefile 
	$(cc) -c -o SpeedGraphic.o SpeedGraphic.c $(opts) 

SpeedTracker.o : SpeedTracker.c Shared.h ../datalib/Shared.h \
  SpeedTracker.h SpeedGraphic.h ../datalib/GPMFdata.h Makefile 
	$(cc) -c -o SpeedTracker.o SpeedTracker.c $(opts) 

lib_gfxgen.a : SpeedTracker.o SpeedGraphic.o PathGraphic.o \
  AltitudeGraphic.o Makefile 
	 ar rcs  lib_gfxgen.a SpeedTracker.o SpeedGraphic.o \
  PathGraphic.o AltitudeGraphic.o 

all: lib_gfxgen.a 
