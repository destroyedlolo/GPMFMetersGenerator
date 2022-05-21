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
#include <iostream>

#include "datalib/Context.h"
#include "datalib/GPVideo.h"
#include "datalib/GPX.h"

#define VERSION "0.01a01"

GPX *Gpx = NULL;	// external original GPX data
uint32_t proximity = 15;	// Proximity threshold 
							// (bellow this distance, places are considered to 
							// be the same.

	/* Extended GPVideo with it's name */
class NamedGPVideo : public GPVideo, public std::string {
public:
	NamedGPVideo( char *n ) : GPVideo(n), std::string(n){}
};

std::vector<NamedGPVideo> videos;

int main(int argc, char *argv[]){
	bool force = false;	// force video inclusion

			/* Reading arguments */
	int opt;
	while(( opt = getopt(argc, argv, ":vdhG:p:F")) != -1){
		switch(opt){
		case 'G':
			Gpx = new GPX(optarg);
			if(verbose)
				Gpx->Dump();
			break;
		case 'd':	// debug implies verbose
			debug = true;
		case 'v':
			verbose = true;
			break;
		case 'p':
			proximity = strtoul(optarg, NULL, 10);
			break;
		case 'F':
			force = true;
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
				"-p<val> : Proximity threshold (default: 15m)\n"
				"-F : force video inclusion\n"
				"\n"
				"-v : turn verbose on\n"
				"-d : turn debugging messages on\n"
			);
			exit(EXIT_FAILURE);
			break;
		}
	}

	if(!Gpx){
		fputs("No GPX file loaded\n", stderr);
		exit(EXIT_FAILURE);
	}

	if(optind >= argc){
		puts("*F* No Video provided");
		exit(EXIT_FAILURE);		
	}

	for(; optind < argc; optind++){
		NamedGPVideo video(argv[optind]);	// load the first chunk

		if(verbose){
			video.Dump();
			printf("*I* Distance vs min : %.0fm, max %.0fm\n",
				video.getMin().Distance(Gpx->getMin()),
				video.getMax().Distance(Gpx->getMax())
			);
		}

		if( !Gpx->sameArea(video.getMin()) ||
			!Gpx->sameArea(video.getMax()) ){
				fprintf(stderr, "*W* This video seems outside the GPX trace (%.0fm and %.0fm)\n", 
					video.getMin().Distance(Gpx->getMin()),
					video.getMax().Distance(Gpx->getMax())
				);

				if(!force){
					fputs("*F* exiting (use -F to force this video inclusion)\n", stderr);
					exit(EXIT_FAILURE);
				}
		}
		videos.push_back(video);
	}

}

