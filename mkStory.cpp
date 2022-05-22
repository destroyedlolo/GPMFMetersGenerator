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

#if 0
double proximity = 1;	// Proximity threshold 
							// (bellow this distance, places are considered to 
#endif							// be the same.

	/* Extended GPVideo */
class GPVideoExt : public GPVideo, public std::string {
public:
	struct closest {
		int idx;
		double distance;

		closest():idx(-1){}
	} beginning, end;

	GPVideoExt( char *n ) : GPVideo(n), std::string(n){}

		/* update beginning & end as per a given value */
	void Consider( int idx, GPSCoordinate &p ){
		if(this->beginning.idx == -1){	// Initialize with the 1st point
			this->beginning.idx = this->end.idx = idx;
			this->beginning.distance = this->getFirst().Distance(p);
			this->end.distance = this->getLast().Distance(p);
		} else {
			double dst = this->getFirst().Distance(p);
			if(dst < this->beginning.distance){	// New guessed beginning point
				this->beginning.idx = idx;
				this->beginning.distance = dst;

				this->end.idx = -1;			// invalidate end point
				this->end.distance = NAN;
			} else {
				dst = this->getLast().Distance(p);
				if(std::isnan(this->end.distance) || dst < this->end.distance){	// new guessed end point
					this->end.idx = idx;
					this->end.distance = dst;
				}
			}
		}
	}
};

std::vector<GPVideoExt> videos;

FILE *story = NULL;

int main(int argc, char *argv[]){
	bool force = false;	// force video inclusion

			/* Reading arguments */
	int opt;
	while(( opt = getopt(argc, argv, ":vdhG:F")) != -1){
		switch(opt){
		case 'G': {
				Gpx = new GPX(optarg);
				if(verbose)
					Gpx->Dump();

				std::string res = optarg;
				size_t pos = res.rfind(".gpx");
				if(pos != std::string::npos)
					res.erase(pos, 4);
				res += ".story";

				printf("*O* Generating story in '%s'\n", res.c_str());

				if(!(story = fopen(res.c_str(), "w"))){
					perror(res.c_str());
					exit(EXIT_FAILURE);		
				}
			}
			break;
		case 'd':	// debug implies verbose
			debug = true;
		case 'v':
			verbose = true;
			break;
#if 0
CAUTION : 'p' has been removed from getopt !!

		case 'p':
			proximity = atof(optarg);
			break;
#endif
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
#if 0
				"-p<val> : Proximity threshold (default: 1m)\n"
#endif
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
		GPVideoExt video(argv[optind]);	// load the first chunk

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

	int idx;
	for( idx = 0; idx < (int)Gpx->getSampleCount(); idx++ ){
		auto &gpx = (*Gpx)[idx];

		for(auto &v : videos)
			v.Consider( idx, gpx );
	}

	puts("*I* results");

	int prev = -1;			// Prev index
	bool issue = false;	// Did we detected an issue ?

//		  GX013158.MP4| 99999 | 100000.0 | 99999 | 100000.0 | 
	puts(" ----------------------------------------------------");
	puts("|    video     |    Beginning     |     End          | Status");
	puts("|              | Index | distance | index | distance |");
	puts(" ------------------------------------------------------------");
	printf("| GPX : %s -> %s\n", Gpx->getMin().strLocalTime().c_str(), Gpx->getMax().strLocalTime().c_str());
	puts(" ------------------------------------------------------------");
	for(auto &v : videos){
		printf("| %12s | %05d | %8.1f | %05d | %8.1f | %s | ",
			basename(v.c_str()),
			v.beginning.idx, v.beginning.distance,
			v.end.idx, v.end.distance,
			v.getFirst().strLocalHour(true).c_str()
		);

		if(v.end.idx == -1){
			puts(" Not ending");
			issue = true;
		} else if(prev > v.beginning.idx){
			puts(" Overlapping");
			issue = true;
		} else
			puts(" ok");
	}

	if(issue){
		fputs("*W* At least Problem has been detected", stderr);
		if(!force){
			fputs("*F* Use '-F' flag to accept it", stderr);
			exit(EXIT_FAILURE);
		}
	}

		/* As we're relying on previous processing, we don't have to check
		 * boundary
		 */
	idx = 0;
	fputs("GPMFStory 1.0\n", story);	// Header to identify a story
	fprintf(story, "s%s\n", Gpx->getMin().strLocalTime().c_str());	// Starting time

	fputs("# Format : Index, latitude, longitude, altitude, timestamps (ignored)\n", story);
	for(int vidx = 0; vidx < (int)videos.size(); vidx++){
		for(; idx < videos[vidx].beginning.idx; idx++){	// GPX before the video
			auto &gpx = (*Gpx)[idx];
			fprintf(story,"p%05d, %f, %f, %f, %s\n",
				idx,
				gpx.getLatitude(), gpx.getLongitude(),
				gpx.getAltitude(), gpx.strLocalTime().c_str()
			);
		}

		fprintf(story, "# video's starting timestamps : %s\n", videos[vidx].getFirst().strLocalTime().c_str() );
		fprintf(story, "v%s\n", videos[vidx].c_str());

		for(; idx < videos[vidx].end.idx; idx++){	// GPX before the video
			auto &gpx = (*Gpx)[idx];
			fprintf(story,"p%05d, %f, %f, %f, %s\n",
				idx,
				gpx.getLatitude(), gpx.getLongitude(),
				gpx.getAltitude(), gpx.strLocalTime().c_str()
			);
		}

		fprintf(story, "# video's ending timestamps : %s\n", videos[vidx].getLast().strLocalTime().c_str() );
		fputs("V\n", story);
	}

	for(; idx < (int)Gpx->getSampleCount(); idx++){	// Remaining
		auto &gpx = (*Gpx)[idx];
		fprintf(story,"p%05d, %f, %f, %f, %s\n",
			idx,
			gpx.getLatitude(), gpx.getLongitude(),
			gpx.getAltitude(), gpx.strLocalTime().c_str()
		);
	}
	
	fclose(story);

}

