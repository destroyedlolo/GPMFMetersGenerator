/* GPMFMetersGenerator
 *
 * Generate images sequence from GPMF' meters
 *
 * 26/01/2022 - Starting development
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "gpmf-parser/GPMF_parser.h"
#include "gpmf-parser/GPMF_utils.h"
#include "gpmf-parser/demo/GPMF_mp4reader.h"

	/* Configuration */

bool verbose = false;
bool debug = false;

static void usage( const char *name ){
	printf("%s [opts] video.mp4\n", name);
	puts(
		"\nKnown opts :\n"
		"-v : turn verbose on\n"
		"-d : turn debugging messages on\n"
	);
}

static const char *gpmfError( GPMF_ERROR err ){
	switch(err){
	case GPMF_OK : return "No problemo";
	case GPMF_ERROR_MEMORY : return "NULL Pointer or insufficient memory";
	case GPMF_ERROR_BAD_STRUCTURE : return "Non-complaint GPMF structure detected";
	case GPMF_ERROR_BUFFER_END : return "reached the end of the provided buffer";
	case GPMF_ERROR_FIND : return "Find failed to return the requested data, but structure is valid";
	case GPMF_ERROR_LAST : return "reached the end of a search at the current nest level";
	case GPMF_ERROR_TYPE_NOT_SUPPORTED : return "a needed TYPE tuple is missing or has unsupported elements";
	case GPMF_ERROR_SCALE_NOT_SUPPORTED : return "scaling for an non-scaling type, e.g. scaling a FourCC field to a float";
	case GPMF_ERROR_SCALE_COUNT : return "A SCAL tuple has a mismatching element count";
	case GPMF_ERROR_UNKNOWN_TYPE : return "Potentially valid data with a new or unknown type";
	case GPMF_ERROR_RESERVED : return "internal usage";
	}
	return "???";
}

int main(int ac, char **av){
	uint8_t nvideo = 1;	/* which argument contains the video */
	uint32_t* payload = NULL;
	uint32_t payloadsize = 0;
	size_t payloadres = 0;

	if(ac < 2){
		usage(av[0]);
		exit(EXIT_FAILURE);
	}

	while( nvideo < ac && *av[nvideo] == '-' ){
		switch( av[nvideo][1] ){
		case 'v':
			verbose = true;
			break;
		case 'd':
			debug = true;
			break;
		default :
			usage(av[0]);
			exit(EXIT_FAILURE);
		}
		nvideo++;
	}

	size_t mp4handle = OpenMP4Source(av[nvideo], MOV_GPMF_TRAK_TYPE, MOV_GPMF_TRAK_SUBTYPE, 0);
	if(!mp4handle){
		printf("*F* '%s' is an invalid MP4/MOV or it has no GPMF data\n\n", av[nvideo]);
		exit(EXIT_FAILURE);
	}

		/* Get framerate and number of frames */
	uint32_t fr_num, fr_dem;
	uint32_t frames = GetVideoFrameRateAndCount(mp4handle, &fr_num, &fr_dem);
	if(!frames){
		puts("*F* Can't get frame count (incorrect MP4 ?)");
		exit(EXIT_FAILURE);
	}
	if(verbose)
		printf("Video framerate : %.3f (%u frames)\n", (float)fr_num / (float)fr_dem, frames);

	uint32_t index, payloads = GetNumberPayloads(mp4handle);
	if(debug)
		printf("*d* payloads : %u\n", payloads);

	for( index = 0; index < payloads; index++ ){
		GPMF_ERR ret;
		double tstart = 0.0, tend = 0.0;	/* Timeframe of this payload */

		payloadsize = GetPayloadSize(mp4handle, index);
		payloadres = GetPayloadResource(mp4handle, payloadres, payloadsize);
		payload = GetPayload(mp4handle, payloadres, index);

		if(debug)
			printf("*d* payload : %u ... ", index);

		if(!payload){	
				/* For the moment, crashing.
				 * Let see in the future if a recovery is needed ...
				 */
			puts("*F* can't get Payload\n");
			exit(EXIT_FAILURE);
		}

		if((ret = GetPayloadTime(mp4handle, index, &tstart, &tend)) != GPMF_OK){
			printf("*F* can't get payload's time : %s\n", gpmfError(ret));
			exit(EXIT_FAILURE);
		}

		if(debug)
			printf("\tfrom %.3f to %.3f seconds\n", tstart, tend);
	}
}
