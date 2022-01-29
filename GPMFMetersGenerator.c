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
	GPMF_stream metadata_stream, * ms = &metadata_stream;

		 /* Read arguments */
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
		double tstart, tend;	/* Timeframe of this payload */

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

		if(verbose || debug)
			printf("from %.3f to %.3f seconds\n", tstart, tend);

		while(GPMF_OK == GPMF_FindNext(ms, STR2FOURCC("STRM"), GPMF_RECURSE_LEVELS|GPMF_TOLERANT)){
			if(GPMF_OK != GPMF_FindNext(ms, STR2FOURCC("GPS5"), GPMF_RECURSE_LEVELS|GPMF_TOLERANT))
				continue;

			char* rawdata = (char*)GPMF_RawData(ms);
			uint32_t key = GPMF_Key(ms);
			GPMF_SampleType type = GPMF_Type(ms);
			uint32_t samples = GPMF_Repeat(ms);
			uint32_t elements = GPMF_ElementsInStruct(ms);

			if(debug)
				printf("*d* %u samples, %u elements\n", samples, elements);

			if(samples){
#define MAX_UNITS	64
#define MAX_UNITLEN	8
				char units[MAX_UNITS][MAX_UNITLEN] = { "" };
				uint32_t unit_samples = 1;

				char complextype[MAX_UNITS] = { "" };
				uint32_t type_samples = 1;

				uint32_t buffersize = samples * elements * sizeof(double);
				GPMF_stream find_stream;
				double *ptr, *tmpbuffer = (double*)malloc(buffersize);

				uint32_t i, j;

				if(!tmpbuffer){
					puts("*F* Can't allocate temporary buffer\n");
					exit(EXIT_FAILURE);
				}

				GPMF_CopyState(ms, &find_stream);
				if(
					GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_SI_UNITS, GPMF_CURRENT_LEVEL | GPMF_TOLERANT) ||
					GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_UNITS, GPMF_CURRENT_LEVEL | GPMF_TOLERANT)
				){
					char *data = (char*)GPMF_RawData(&find_stream);
					uint32_t ssize = GPMF_StructSize(&find_stream);
					if (ssize > MAX_UNITLEN - 1) ssize = MAX_UNITLEN - 1;
					unit_samples = GPMF_Repeat(&find_stream);

					for(i = 0; i < unit_samples && i < MAX_UNITS; i++){
						memcpy(units[i], data, ssize);
						units[i][ssize] = 0;
						data += ssize;
					}

						/* Search for TYPE if Complex */
					GPMF_CopyState(ms, &find_stream);
					type_samples = 0;
					if(GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_TYPE, GPMF_CURRENT_LEVEL | GPMF_TOLERANT)){
						char* data = (char*)GPMF_RawData(&find_stream);
						uint32_t ssize = GPMF_StructSize(&find_stream);
						if(ssize > MAX_UNITLEN - 1) ssize = MAX_UNITLEN - 1;
						type_samples = GPMF_Repeat(&find_stream);

						for(i = 0; i < type_samples && i < MAX_UNITS; i++)
							complextype[i] = data[i];

					}

					if(GPMF_OK == GPMF_ScaledData(ms, tmpbuffer, buffersize, 0, samples, GPMF_TYPE_DOUBLE)){	/* Output scaled data as floats */
						ptr = tmpbuffer;
						int pos = 0;
						for(i = 0; i < samples; i++){
							if(verbose)
								printf("\t%c%c%c%c ", PRINTF_4CC(key));
							
							for (j = 0; j < elements; j++){
								if(type == GPMF_TYPE_STRING_ASCII){
									if(verbose)
										printf("(char)%c", rawdata[pos]);
									pos++;
									ptr++;
								} else if(type_samples == 0){ /* no TYPE structure */
									if(verbose)
										printf("(no type)%.3f%s, ", *ptr++, units[j % unit_samples]);
								} else if(complextype[j] != 'F'){
									if(verbose)
										printf("(F)%.3f%s, ", *ptr++, units[j % unit_samples]);
									pos += GPMF_SizeofType((GPMF_SampleType)complextype[j]);
								} else if (type_samples && complextype[j] == GPMF_TYPE_FOURCC){
									ptr++;
									if(verbose)
										printf("(4CC)%c%c%c%c, ", rawdata[pos], rawdata[pos + 1], rawdata[pos + 2], rawdata[pos + 3]);
									pos += GPMF_SizeofType((GPMF_SampleType)complextype[j]);
								}
							}

							if(verbose)
								puts("");
						}
					}

				}

				free(tmpbuffer);
			}
		}
		GPMF_ResetState(ms);
	}
}
