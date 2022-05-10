all:
	$(MAKE) -f GPMFMakeFile
	$(MAKE) -C datalib
	$(MAKE) -C gfxlib
	$(MAKE) -f MakefileLocal
