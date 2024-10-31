#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <libgen.h>

#include "datalib/Context.h"
#include "datalib/GPVideo.h"
#include "datalib/GPX.h"
#include "gfxlib/Export.h"

	/* Configuration */

void gopro2gpx(const char* provided_file_name, const char* gpx_filename);