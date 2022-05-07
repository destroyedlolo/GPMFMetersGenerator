# makefile created automaticaly by LFMakeMaker
# LFMakeMaker 1.6 (May  7 2022 20:46:23) (c)LFSoft 1997

gotoall: all


#The compiler (may be customized for compiler's options).
cc=gcc -Wall -O2
opts=-lm $(shell pkg-config --cflags cairo ) $(shell pkg-config --libs cairo )

AltitudeGraphic.o : AltitudeGraphic.c Shared.h AltitudeGraphic.h \
  GPMFdata.h GpxHelper.h Makefile 
	$(cc) -c -o AltitudeGraphic.o AltitudeGraphic.c $(opts) 

GPMFdata.o : GPMFdata.c Shared.h GPMFdata.h Makefile 
	$(cc) -c -o GPMFdata.o GPMFdata.c $(opts) 

GpxHelper.o : GpxHelper.c Shared.h GpxHelper.h Makefile 
	$(cc) -c -o GpxHelper.o GpxHelper.c $(opts) 

PathGraphic.o : PathGraphic.c Shared.h PathGraphic.h GPMFdata.h \
  GpxHelper.h Makefile 
	$(cc) -c -o PathGraphic.o PathGraphic.c $(opts) 

SpeedGraphic.o : SpeedGraphic.c SpeedGraphic.h GPMFdata.h Shared.h \
  Makefile 
	$(cc) -c -o SpeedGraphic.o SpeedGraphic.c $(opts) 

SpeedTracker.o : SpeedTracker.c Shared.h SpeedTracker.h SpeedGraphic.h \
  GPMFdata.h Makefile 
	$(cc) -c -o SpeedTracker.o SpeedTracker.c $(opts) 

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

datalib : ../gpmf-parser/demo/GPMF_mp4reader.o \
  ../gpmf-parser/GPMF_utils.o ../gpmf-parser/GPMF_parser.o \
  SpeedTracker.o SpeedGraphic.o PathGraphic.o GpxHelper.o GPMFdata.o \
  AltitudeGraphic.o Makefile 
	 ar rcs  datalib ../gpmf-parser/demo/GPMF_mp4reader.o \
  ../gpmf-parser/GPMF_utils.o ../gpmf-parser/GPMF_parser.o \
  SpeedTracker.o SpeedGraphic.o PathGraphic.o GpxHelper.o GPMFdata.o \
  AltitudeGraphic.o 

all: datalib 
