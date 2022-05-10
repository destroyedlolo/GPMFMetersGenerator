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
			if(GPMF_OK == GPMF_FindNext(ms, STR2FOURCC("GPSU"), (GPMF_LEVELS)(GPMF_RECURSE_LEVELS|GPMF_TOLERANT) )){	// find out GPS time if any
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

					/* Convert to timestamp */
				time = mktime( &t );	// mktime() is based on local TZ
				time += t.tm_gmtoff;	// reverting back to Z (as it's GPS TZ)

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

			if(GPMF_OK != GPMF_FindNext(ms, STR2FOURCC("GPS5"), (GPMF_LEVELS)(GPMF_RECURSE_LEVELS|GPMF_TOLERANT) ))	// No GPS data in this stream ... skipping
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
				if(GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_TYPE, (GPMF_LEVELS)(GPMF_CURRENT_LEVEL | GPMF_TOLERANT) ))
					type_samples = GPMF_Repeat(&find_stream);

				if(type_samples){
					printf("*E* Malformed GPMF : type_samples expected to be 0, got %d.\n", type_samples);
					continue;
				}

				if(
					GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_SI_UNITS, (GPMF_LEVELS)(GPMF_CURRENT_LEVEL | GPMF_TOLERANT) ) ||
					GPMF_OK == GPMF_FindPrev(&find_stream, GPMF_KEY_UNITS, (GPMF_LEVELS)(GPMF_CURRENT_LEVEL | GPMF_TOLERANT) )
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
								tmpbuffer[i*elements + 4],	/* speed3d */
								time
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
}

double GPVideo::addSample( double sec, double lat, double lgt, double alt, double s2d, double s3d, time_t time ){
	double ret=0;

		/* Convert speed from m/s to km/h */
	s2d *= 3.6;
	s3d *= 3.6;

		/* update Min / Max */
	if(!first){	/* First data */
		this->min.latitude = this->max.latitude = lat;
		this->min.longitude = this->max.longitude = lgt;
		this->min.altitude = this->max.altitude = alt;
		this->min.spd2d = this->max.spd2d = s2d;
		this->min.spd3d = this->max.spd3d = s3d;
		this->min.sample_time = this->max.sample_time = time;
	} else {
		if(lat < this->min.latitude)
			this->min.latitude = lat;
		if(lat > this->max.latitude)
			this->max.latitude = lat;

		if(lgt < this->min.longitude)
			this->min.longitude = lgt;
		if(lgt > this->max.longitude)
			this->max.longitude = lgt;

		if(alt < this->min.altitude)
			this->min.altitude = alt;
		if(alt > this->max.altitude)
			this->max.altitude = alt;

		if(s2d < this->min.spd2d)
			this->min.spd2d = s2d;
		if(s2d > this->max.spd2d)
			this->max.spd2d = s2d;

		if(s3d < this->min.spd3d)
			this->min.spd3d = s3d;
		if(s3d > this->max.spd3d)
			this->max.spd3d = s3d;

		if(time != (time_t)-1){
			if(this->min.sample_time == (time_t)-1 || this->min.sample_time > time)
				this->min.sample_time = time;
			if(this->max.sample_time == (time_t)-1 || this->max.sample_time < time)
				this->max.sample_time = time;
		}
	}

		/* manage timing */
	this->lastTiming = sec;
	sec += this->voffset;	// shift by this part's offset

	if(!this->first || sec >= this->nextsample){	// store a new sample
		if(this->first && sec > this->nextsample + SAMPLE/2)	// Drifting
			ret = sec - (this->nextsample + SAMPLE/2);

		this->nextsample += SAMPLE;
		if(debug)
			printf("accepted : %f, next:%f\n", sec, this->nextsample);

		struct GPMFdata *nv = new GPMFdata( lat, lgt, alt, s2d, s3d, time);

			/* insert the new sample in the list */
		if(!this->first)
			this->first = nv;
		else
			this->last->next = nv;
		this->last = nv;

		this->samples_count++;
	}

	return ret;
}

GPVideo::GPVideo( char *fch ) : first(NULL), last(NULL), nextsample(0), voffset(0) {
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

void GPVideo::AddPart( char * ){
}

void GPVideo::Dump( void ){
	puts("*I* Video min/max :");
	printf("\tLatitude : %f deg - %f deg (%f)\n", min.latitude, max.latitude, max.latitude - min.latitude);
	printf("\tLongitude : %f deg - %f deg (%f)\n", min.longitude, max.longitude, max.longitude - min.longitude);
	printf("\tAltitude : %.3f m - %.3f m (%.3f)\n", min.altitude, max.altitude, max.altitude - min.altitude);
	printf("\tSpeed2d : %.3f km/h - %.3f km/h (%.3f)\n", min.spd2d, max.spd2d, max.spd2d - min.spd2d);
	printf("\tSpeed3d : %.3f km/h - %.3f km/h (%.3f)\n", min.spd3d, max.spd3d, max.spd3d - min.spd3d);
	struct tm *t = gmtime(&min.sample_time);
	printf("\tTime : ");
	printtm(t);
	printf(" -> ");
	t = gmtime(&max.sample_time);
	printtm(t);
	puts("");
}
