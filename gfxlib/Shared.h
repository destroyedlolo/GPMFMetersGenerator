/* Shared objects for graphics generation
 */

#ifndef SHAREDGFX_H
#define SHAREDGFX_H

#include "../datalib/Shared.h"

extern bool video;

	/* Generate video from given images sequence
	 * -> fulltarget, filename : full path and filename pointer
	 * -> iname : images' 3char name
	 * -> vname : target video name 
	 */
extern void generateVideo( const char *fulltarget, char *filename, const char *iname, const char *vname);


#endif
