/* mkStory.cpp
 *
 * Synchronize GPX files with a collection of GoPro's video
 *
 * 16/05/2022 - Starting development
 */
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cassert>
#include <cstring>

#include <vector>

#include "datalib/Context.h"
#include "datalib/GPVideo.h"
#include "datalib/GPX.h"

#define VERSION "0.01a01"

GPX *gpx = NULL;	// external original GPX data

std::vector<GPVideo> videos;

int main(int argc, char *argv[]){
			/* Reading arguments */
	int opt;
	while(( opt = getopt(argc, argv, ":vdhG:")) != -1){
		switch(opt){
		case 'G':
			gpx = new GPX(optarg);
			if(verbose)
				gpx->Dump();
			break;
		case 'd':	// debug implies verbose
			debug = true;
		case 'v':
			verbose = true;
			break;
		case '?':	// unknown option
			printf("unknown option: -%c\n", optopt);
		case 'h':
		case ':':	// no argument provided (or missing argument)
			puts(
				"\nmkStory v" VERSION "\n"
				"(c) L.Faillie (destroyedlolo) 2022\n"
				"\n"
				"mkStory [-options] video.mp4 ...\n"
				"Known opts :\n"
				"-G<file> : GPX of the hicking\n"
				"\n"
				"-v : turn verbose on\n"
				"-d : turn debugging messages on\n"
			);
			exit(EXIT_FAILURE);
			break;
		}
	}

	if(!gpx){
		fputs("No GPX file loaded\n", stderr);
		exit(EXIT_FAILURE);
	}

	if(optind >= argc){
		puts("*F* No Video provided");
		exit(EXIT_FAILURE);		
	}

	for(; optind < argc; optind++){
		printf("*I Reading '%s'\n", argv[optind]);

			/* As GoPro's OpenMP4Source() needs a "char *",
			 * it's easier to manage this temporary file in C
			 * instead of a C++'s string
			 */
		char *fname = strdup(argv[optind]);
		assert(fname);		// quick & dirty : no raison to fail

		GPVideo video(argv[optind]);	// load the first chunk

			// "GX013561.MP4" -> 12 chars
		size_t len = strlen(fname);
		if(len < 12){
			fputs("*E* filename doesn't correspond to a GoPro video\n", stderr);
			exit(EXIT_FAILURE);
		}
		if(strncmp(fname + len - 12, "GX01", 4)){
			fputs("*E* not a GoPro video or not the 1st one\n", stderr);
			exit(EXIT_FAILURE);
		}
		len -= 10;	// point to the part number
	
		for(unsigned int i = 2; i<99; i++){	// As per GoPro, up to 98 parts
			char buff[3];
			sprintf(buff, "%02d", i);
			memcpy(fname + len, buff, 2);

			if(!access(fname, R_OK)){
				printf("*I Adding '%s'\n", argv[optind]);
				
				video.AddPart(fname);
			}
		}

		if(verbose)
			video.Dump();

		videos.push_back(video);

		free(fname);
	}

}

