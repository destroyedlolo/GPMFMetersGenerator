/* GPVideo
 * 	Impersonates GoPro video
 *
 * 	Notez-bien : as of writing, every errors are fatal
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
#include "../gpmf-parser/GPMF_parser.h"
#include "../gpmf-parser/GPMF_utils.h"
#include "../gpmf-parser/demo/GPMF_mp4reader.h"
}

#include "Context.h"
#include "GPVideo.h"

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

void GPVideo::readGPMF( void ){
	uint32_t payloads = GetNumberPayloads(this->mp4handle);
	GPMF_stream metadata_stream, * ms = &metadata_stream;
	size_t payloadres = 0;
	time_t time = (time_t)-1;

	if(debug)
		printf("*d* payloads : %u\n", payloads);

	for(uint32_t index = 0; index < payloads; index++){
		GPMF_ERROR ret;
		double tstart, tend, tstep;	// Timeframe of this payload

		uint32_t payloadsize = GetPayloadSize(this->mp4handle, index);
		payloadres = GetPayloadResource(this->mp4handle, payloadres, payloadsize);
		uint32_t *payload = GetPayload(this->mp4handle, payloadres, index);

		if(debug)
			printf("*d* payload : %u ... ", index);

		if(!payload){	
			puts("*F* can't get Payload\n");
			exit(EXIT_FAILURE);
		}

		if((ret = (GPMF_ERROR)GetPayloadTime(this->mp4handle, index, &tstart, &tend)) != GPMF_OK){
			printf("*F* can't get payload's time : %s\n", gpmfError(ret));
			exit(EXIT_FAILURE);
		}

		if((ret = (GPMF_ERROR)GPMF_Init(ms, payload, payloadsize)) != GPMF_OK){
			printf("*F* can't init GPMF : %s\n", gpmfError(ret));
			exit(EXIT_FAILURE);
		}

		if(debug)
			printf("from %.3f to %.3f seconds\n", tstart, tend);

		while(GPMF_OK == GPMF_FindNext(ms, STR2FOURCC("STRM"), (GPMF_LEVELS)(GPMF_RECURSE_LEVELS|GPMF_TOLERANT) )){	// Looks for stream
			if(GPMF_OK == GPMF_FindNext(ms, STR2FOURCC("GPSU"), (GPMF_LEVELS)(GPMF_RECURSE_LEVELS|GPMF_TOLERANT) )){	// find out GPS time
				if(GPMF_Type(ms) != GPMF_TYPE_UTC_DATE_TIME){
					puts("*E* found GPSU which doesn't contain a date");
					continue;
				}

				if(GPMF_StructSize(ms) != 16){
					puts("*E* found GPSU but its structure size is not 16");
					continue;
				}

				const char *p = (const char *)GPMF_RawData(ms);
				if(debug){
					char t[17];
					t[16] = 0;

					strncpy(t, p, 16);
					printf("*I* GPSU : '%s'\n", t);
				}
	
				struct tm t;
				memset(&t, 0, sizeof(struct tm));
				t.tm_year = char2int(p) + 100;
				t.tm_mon = char2int(p += 2) - 1;
				t.tm_mday = char2int(p += 2);
				t.tm_hour = char2int(p += 2);
				t.tm_min = char2int(p += 2);
				t.tm_sec = char2int(p += 2);
				t.tm_isdst = -1;

					/* Convert to timestamp and revert local TZ */
				time = mktime( &t );
				time += t.tm_gmtoff;

				if(debug){
					printf("GPS time : %4d-%02d-%02d %02d:%02d:%02d (offset %6ld sec) -> ", 
						t.tm_year+1900, t.tm_mon+1, t.tm_mday,
						t.tm_hour, t.tm_min, t.tm_sec, t.tm_gmtoff
					);

					struct tm *tres = gmtime(&time);
					printtm(tres);
					puts("");
				}
			}
		}
		GPMF_ResetState(ms);
	}
}

GPVideo::GPVideo( char *fch ){
	this->mp4handle = OpenMP4Source(fch, MOV_GPMF_TRAK_TYPE, MOV_GPMF_TRAK_SUBTYPE, 0);
	if(!this->mp4handle){
		printf("*F* '%s' is an invalid MP4/MOV or it has no GPMF data\n\n", fch);
		exit(EXIT_FAILURE);
	}

	uint32_t frames = GetVideoFrameRateAndCount(this->mp4handle, &this->fr_num, &this->fr_dem);
	if(!frames){
		puts("*F* Can't get frame count (incorrect MP4 ?)");
		exit(EXIT_FAILURE);
	}
	if(verbose)
		printf("*I* Video framerate : %.3f (%u frames)\n", (float)this->fr_num / (float)this->fr_dem, frames);

	this->readGPMF();
}
