/* Export.h
 * Export GPMF as standard GPS files
 */
#ifndef EXPORT_H
#define EXPORT_H

#include "../datalib/Context.h"
#include "../datalib/GPVideo.h"

class Export {
	GPVideo &video;	// source video

public:
	Export( GPVideo &v ): video(v) {}

	void generateGPX( const char *fulltarget, char *filename, char *iname );
};

#endif
