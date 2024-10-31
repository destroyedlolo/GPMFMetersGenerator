all:
	$(MAKE) -f GPMFMakeFile
	$(MAKE) -C datalib
	$(MAKE) -C gfxlib
	$(MAKE) -f MakefileLocal
clean:
	rm -f GPMFMetersGenerator gpmf2gpx libGPMF.a libdatalib.a libgfxlib.a
	rm -f gpmf-parser/*.o gpmf-parser/demo/*.o
	rm -f datalib/*.o
	rm -f gfxlib/*.o