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
#include "gpmf2gpx.h"

	/* Configuration */

#include "Version.h"

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("USAGE: %s [mp4file] [output GPX]\n", argv[0]);
  } else if(argc == 2) {
    size_t mylen = strlen(argv[1]) + 5 + 1;
    char target[ mylen ];
    char myTarget[ mylen ];
    strcpy(myTarget, argv[1]);
    char *extension = strrchr(myTarget, '.');
    *extension = 0;
    snprintf(target, mylen, "%s_test.gpx", myTarget);
    printf("%s", target);
    gopro2gpx(argv[1], target);
  } else {
    gopro2gpx(argv[1], argv[2]);
  }  
}

void gopro2gpx(const char* provided_file_name, const char* gpx_filename) {
  char * mp4 = strdup(provided_file_name);
  GPVideo video(mp4);
	video.Dump();
  Export gfx( video );
  printf("%s", gpx_filename);
  gfx.generateGPX(gpx_filename, 1);
}