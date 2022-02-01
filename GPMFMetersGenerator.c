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
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

#include "gpmf-parser/GPMF_parser.h"
#include "gpmf-parser/GPMF_utils.h"
#include "gpmf-parser/demo/GPMF_mp4reader.h"

#include "GPMFdata.h"
#include "AltitudeGraphic.h"
#include "SpeedGraphic.h"

	/* Configuration */

bool verbose = false;
bool debug = false;
bool altitude = true;
bool speed = true;
bool force = false;

static void usage( const char *name ){
	printf("%s [opts] video.mp4\n", name);
	puts(
		"\nGPMFMetersGenerator v0.1\n"
		"(c) L.Faillie (destroyedlolo) 2022\n"
		"\nKnown opts :\n"
		"-a : disable altitude gfx generation\n"
		"-s : disable speed gfx generation\n"
		"-3 : uses 3d speed (2d by default)\n"
		"-F ! don't fail if the target directory exists\n"
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
	GPMF_stream metadata_stream, * ms = &metadata_stream;

		 /* Read arguments */
	if(ac < 2){
		usage(av[0]);
		exit(EXIT_FAILURE);
	}

	while( nvideo < ac && *av[nvideo] == '-' ){
		switch( av[nvideo][1] ){
		case 'a':
			altitude = false;
			break;
		case 's':
			speed = false;
			break;
		case '3':
			s3d = true;
			break;
		case 'F':
			force = true;
			break;
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

		/* Open and validate the file */
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
		printf("*I* Video framerate : %.3f (%u frames)\n", (float)fr_num / (float)fr_dem, frames);


		/* Determine target directory name */
	int len = strlen(av[nvideo]) + strlen("img0123456.png") + 1;
	char targetDir[ len ];						/* Where files will be created */
	strcpy(targetDir, av[nvideo]);
	char *targetFile = strrchr(targetDir, '.');	/* target file name */
	if(!targetFile){
		printf("*F* Video filename doesn't have extension");
		exit(EXIT_FAILURE);
	}

		/* Create the target directory */
	*targetFile = 0;
	if(!force){
		DIR* dir = opendir(targetDir);
		if(dir){
			closedir(dir);
			printf("*F* '%s' alreay exists\n", targetDir);
			exit(EXIT_FAILURE);
		} else if(errno != ENOENT){
			perror(targetDir);
			exit(EXIT_FAILURE);
		}
	}

	if(mkdir(targetDir, S_IRWXU | S_IRWXG | S_IRWXO) == -1){
		perror(targetDir);
		if(!force)
			exit(EXIT_FAILURE);
	}

		/* done with directory */
	*(targetFile++) = '/';
	*targetFile = 0;
	if(verbose || debug)
		printf("*I* images will be generated in '%s'\n", targetDir);


		/* Reading data */
	uint32_t index, payloads = GetNumberPayloads(mp4handle);
	if(debug)
		printf("*d* payloads : %u\n", payloads);

	for( index = 0; index < payloads; index++ ){
		GPMF_ERR ret;
		double tstart, tend, tstep;	/* Timeframe of this payload */

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

		if((ret = GPMF_Init(ms, payload, payloadsize)) != GPMF_OK){
			printf("*F* can't init GPMF : %s\n", gpmfError(ret));
			exit(EXIT_FAILURE);
		}

		if(debug)
			printf("from %.3f to %.3f seconds\n", tstart, tend);


		while(GPMF_OK == GPMF_FindNext(ms, STR2FOURCC("STRM"), GPMF_RECURSE_LEVELS|GPMF_TOLERANT)){
			if(GPMF_OK != GPMF_FindNext(ms, STR2FOURCC("GPS5"), GPMF_RECURSE_LEVELS|GPMF_TOLERANT))
				continue;

			uint32_t samples = GPMF_Repeat(ms);
			uint32_t elements = GPMF_ElementsInStruct(ms);

			if(debug)
				printf("*d* %u samples, %u elements\n", samples, elements);
	
			if(elements != 5){
				printf("*E* Malformed GPMF : 5 elements experted by sample, got %d.\n", elements);
				continue;
			}

			if(samples){
				uint32_t type_samples = 1;

				uint32_t buffersize = samples * elements * sizeof(double);
				GPMF_stream find_stream;
				double *tmpbuffer = (double*)malloc(buffersize);

				uint32_t i;

				tstep = (tend - tstart)/samples;

				if(!tmpbuffer){
					puts("*F* Can't allocate temporary buffer\n");
					exit(EXIT_FAILURE);
				}

				GPMF_CopyState(ms, &find_stream);
				type_samples = 0;
				if(GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_TYPE, GPMF_CURRENT_LEVEL | GPMF_TOLERANT))
					type_samples = GPMF_Repeat(&find_stream);

				if(type_samples){
					printf("*E* Malformed GPMF : type_samples expected to be 0, got %d.\n", type_samples);
					continue;
				}

				if(
					GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_SI_UNITS, GPMF_CURRENT_LEVEL | GPMF_TOLERANT) ||
					GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_UNITS, GPMF_CURRENT_LEVEL | GPMF_TOLERANT)
				){
					if(GPMF_OK == GPMF_ScaledData(ms, tmpbuffer, buffersize, 0, samples, GPMF_TYPE_DOUBLE)){	/* Output scaled data as floats */
						for(i = 0; i < samples; i++){
							double drift;

							if(debug)
								printf("t:%.3f l:%.3f l:%.3f a:%.3f 2d:%.3f 3d:%.3f\n",
									tstart + i*tstep,
									tmpbuffer[i*elements + 0],	/* latitude */
									tmpbuffer[i*elements + 1],	/* longitude */
									tmpbuffer[i*elements + 2],	/* altitude */
									tmpbuffer[i*elements + 3],	/* speed2d */
									tmpbuffer[i*elements + 4]	/* speed3d */
								);

							if(!!(drift = addSample(
								tstart + i*tstep,
								tmpbuffer[i*elements + 0],	/* latitude */
								tmpbuffer[i*elements + 1],	/* longitude */
								tmpbuffer[i*elements + 2],	/* altitude */
								tmpbuffer[i*elements + 3],	/* speed2d */
								tmpbuffer[i*elements + 4]	/* speed3d */
							))){
								if(verbose)
									printf("*W* %.3f seconds : data drifting by %.3f\n", tstart + i*tstep, drift);
							}
						}
					}

				}

				free(tmpbuffer);
			}
		}
		GPMF_ResetState(ms);
	}

	dumpSample();

	if(!samples_count){
		puts("*W* No data");
		exit(EXIT_SUCCESS);
	}

	if(altitude)
		GenerateAllAltitudeGfx( targetDir, targetFile );

	if(speed)
		GenerateAllSpeedGfx( targetDir, targetFile );
}
