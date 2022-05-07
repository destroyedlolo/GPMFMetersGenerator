# makefile created automaticaly by LFMakeMaker
# LFMakeMaker 1.6 (May  7 2022 20:46:23) (c)LFSoft 1997

gotoall: all


#The compiler (may be customized for compiler's options).
cc=gcc -Wall -O2
opts=-lm -L./datalib/ -l_datalib $(shell pkg-config --cflags cairo ) $(shell pkg-config --libs cairo )

GPMFMetersGenerator.o : GPMFMetersGenerator.c \
  gpmf-parser/GPMF_parser.h gpmf-parser/GPMF_utils.h \
  gpmf-parser/demo/GPMF_mp4reader.h datalib/Shared.h \
  datalib/GPMFdata.h datalib/AltitudeGraphic.h datalib/SpeedGraphic.h \
  datalib/SpeedTracker.h datalib/PathGraphic.h datalib/GpxHelper.h \
  Makefile 
	$(cc) -c -o GPMFMetersGenerator.o GPMFMetersGenerator.c \
  $(opts) 

GPMFMetersGenerator : GPMFMetersGenerator.o Makefile 
	 $(cc) -o GPMFMetersGenerator GPMFMetersGenerator.o $(opts) 

all: GPMFMetersGenerator 
	$(MAKE) -C datalib
