/* GPVideo
 * 	Impersonates GoPro video
 *
 * 	Notez-bien : as of writing, every errors are fatal
 */

#include <cstdio>
#include <cstdlib>

extern "C" {
#include "../gpmf-parser/GPMF_parser.h"
#include "../gpmf-parser/GPMF_utils.h"
#include "../gpmf-parser/demo/GPMF_mp4reader.h"
}

#include "Context.h"
#include "GPVideo.h"

GPVideo::GPVideo( char *fch ){
	size_t mp4handle = OpenMP4Source(fch, MOV_GPMF_TRAK_TYPE, MOV_GPMF_TRAK_SUBTYPE, 0);
	if(!mp4handle){
		printf("*F* '%s' is an invalid MP4/MOV or it has no GPMF data\n\n", fch);
		exit(EXIT_FAILURE);
	}

	uint32_t frames = GetVideoFrameRateAndCount(mp4handle, &this->fr_num, &this->fr_dem);
	if(!frames){
		puts("*F* Can't get frame count (incorrect MP4 ?)");
		exit(EXIT_FAILURE);
	}
	if(verbose)
		printf("*I* Video framerate : %.3f (%u frames)\n", (float)this->fr_num / (float)this->fr_dem, frames);
}
