/* GPMFMetersGenerator
 *
 * Generate images sequence from GPMF' meters
 *
 * 26/01/2022 - Starting development
 * 28/02/2022 - Handle multipart video
 * 07/03/2022 - Add GPX generation
 * 16/04/2022 - Bump to v1.0 (CAUTION, verbose and debug options changed !)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>

#include "gpmf-parser/GPMF_parser.h"
#include "gpmf-parser/GPMF_utils.h"
#include "gpmf-parser/demo/GPMF_mp4reader.h"

#include "Shared.h"
#include "GPMFdata.h"
#include "AltitudeGraphic.h"
#include "SpeedGraphic.h"
#include "SpeedTracker.h"
#include "PathGraphic.h"
#include "GpxHelper.h"

	/* Configuration */

#define VERSION "1.03"

bool verbose = false;
bool debug = false;
bool altitude = true;
bool speed = true;
bool stracker = true;
bool path = true;
bool force = false;
bool video = false;
bool gpx = false;
bool kml = false;

	/* Helpers */
void generateVideo( const char *fulltarget, char *filename, const char *iname, const char *vname){
	*filename = 0;

	char buf[1024];
	sprintf(buf, "ffmpeg -y -framerate 9 -i %s%s%%07d.png -vcodec png %s%s.mov",
		fulltarget, iname, fulltarget, vname
	);

	int ret = system(buf);
	if(ret){
		printf("*E* returned %d error code\n", ret);
		return;
	}

		/* remove images */
	sprintf(buf,"rm %s%s*.png", fulltarget, iname);
	ret = system(buf);
	if(ret)
		printf("*E* returned %d error code\n", ret);
}

void generateGPX( const char *fulltarget, char *filename, char *iname ){
	sprintf(filename, "%s.gpx", iname);

	FILE *f = fopen(fulltarget,"w");
	if(!f){
		perror(fulltarget);
		return;
	}

	fputs("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
		"<gpx xmlns=\"http://www.topografix.com/GPX/1/1\" xmlns:gpxtpx=\"http://www.garmin.com/xmlschemas/TrackPointExtension/v1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" version=\"1.1\" creator=\"GPMFMetersGenerator\" xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd http://www.garmin.com/xmlschemas/TrackPointExtension/v1 http://www.garmin.com/xmlschemas/TrackPointExtensionv1.xsd\">\n", f);
	fprintf(f, "\t<metadata>\n"
	    "\t\t<name>%s</name>\n"
  		"\t</metadata>\n", iname
	);
	fprintf(f, "\t<trk>\n"
	    "\t\t<name>%s</name>\n"
		"\t\t<trkseg>\n", iname
	);
	
	struct GPMFdata *p;
	for(p = first; p; p = p->next){
		fprintf(f, "\t\t\t<trkpt lat=\"%f\" lon=\"%f\">\n", p->latitude, p->longitude);
		fprintf(f, "\t\t\t\t<ele>%f</ele>\n", p->altitude);
		fputs("\t\t\t</trkpt>\n", f);
	}

	fputs( "\t\t</trkseg>\n"
		"\t</trk>\n"
		"</gpx>\n", f);

	fclose(f);

	if(verbose)
		printf("'%s' generated\n", fulltarget);
}

void generateKML( const char *fulltarget, char *filename, char *iname ){
	sprintf(filename, "%s.kml", iname);

	FILE *f = fopen(fulltarget,"w");
	if(!f){
		perror(fulltarget);
		return;
	}


	fputs("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
		"<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"
		"\t<Document>\n"
	    "\t\t<name>telemetry</name>\n"
    	"\t\t<description>Created by GPMFMetersGenerator v"VERSION"</description>\n"
	, f);

		/* colors */
	fputs("\t\t<Style id=\"trace\">\n"
		"\t\t\t<LineStyle>\n"
        "\t\t\t\t<color>961400FF</color>\n"
        "\t\t\t\t<width>3</width>\n"
      	"\t\t\t</LineStyle>\n"
    	"\t\t</Style>\n"
	, f);

	fputs("\t\t<Style id=\"End\">\n"
		"\t\t\t<LineStyle>\n"
        "\t\t\t\t<color>FFEE58FF</color>\n"
        "\t\t\t\t<width>3</width>\n"
      	"\t\t\t</LineStyle>\n"
    	"\t\t</Style>\n"
	, f);

		/* Trace itself */
	fprintf(f, 
		"\t\t<Folder>\n"
		"\t\t\t<name>Trace</name>\n"
		"\t\t\t<open>1</open>\n"
		"\t\t\t<Placemark>\n"
		"\t\t\t\t<visibility>1</visibility>\n"
		"\t\t\t\t<styleUrl>#trace</styleUrl>\n"
		"\t\t\t\t<name>%s</name>\n"
		"\t\t\t\t<LineString>\n"
		"\t\t\t\t\t<tessellate>1</tessellate>\n"
		"\t\t\t\t\t<altitudeMode>clampToGround</altitudeMode>\n"
		"\t\t\t\t\t<coordinates>"
	, iname);

	struct GPMFdata *p;
	for(p = first; p; p = p->next){
		fprintf(f, "%f,%f,%f\n", p->longitude, p->latitude, p->altitude);
	}

	fputs(
		"</coordinates>\n"
		"\t\t\t\t</LineString>\n"
		"\t\t\t</Placemark>\n"
	, f);

	fprintf(f,
		"\t\t\t<Placemark>\n"
/*		"\t\t\t<styleUrl>#depart</styleUrl>\n" */
		"\t\t\t\t<name>Starting point</name>\n"
		"\t\t\t\t<Point>\n"
		"\t\t\t\t\t<coordinates>%f,%f</coordinates>\n"
		"\t\t\t\t</Point>\n"
		"\t\t\t</Placemark>\n"
	, first->longitude, first->latitude);

	fprintf(f,
		"\t\t\t<Placemark>\n"
		"\t\t\t<styleUrl>#End</styleUrl>\n"
		"\t\t\t\t<name>Finish</name>\n"
		"\t\t\t\t<Point>\n"
		"\t\t\t\t\t<coordinates>%f,%f</coordinates>\n"
		"\t\t\t\t</Point>\n"
		"\t\t\t</Placemark>\n"
	, last->longitude, last->latitude);

	fputs(
		"\t\t</Folder>\n"
		"\t</Document>\n"
		"</kml>"
	, f);

	fclose(f);

	if(verbose)
		printf("'%s' generated\n", fulltarget);
}

	/* main */
static void usage( const char *name ){
	printf("%s [opts] video.mp4 [other video ...]\n", name);
	puts(
		"\nGPMFMetersGenerator v"VERSION"\n"
		"(c) L.Faillie (destroyedlolo) 2022\n"
		"\nKnown opts :\n"
		"-a : disable altitude gfx generation\n"
		"-p : disable path generation\n"
		"-s : disable speed-o-meter generation\n"
		"+3 : uses 3d speed (2d by default)\n"
		"+b : generate both 2d and 3d (in tracker, 3d will be displayed) \n"
		"-t : disable speed tracker\n"
		"-F : don't fail if the target directory exists\n"
		"+V : Generate video and clean images\n"
		"+G<file> : load a GPX file\n"
		"+X : generate GPX file from video(s) telemetry\n"
		"+K : generate KML file from video(s) telemetry\n"
		"+v : turn verbose on\n"
		"+d : turn debugging messages on\n"
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

	while( nvideo < ac && (*av[nvideo] == '-' || *av[nvideo] == '+') ){
		if(*av[nvideo] == '-')
			switch( av[nvideo][1] ){
			case 'a':
				altitude = false;
				break;
			case 'p':
				path = false;
				break;
			case 's':
				speed = false;
				break;
			case 't':
				stracker = false;
				break;
			case 'F':
				force = true;
				break;
			default :
				usage(av[0]);
				exit(EXIT_FAILURE);
			}
		else
			switch( av[nvideo][1] ){
			case '3':
				s3d = true;
				break;
			case 'b':
				sboth = true;
				s3d = true;	/* both implies to have 3d as well */
				break;
			case 'V':
				video = true;
				break;
			case 'G':
				loadGPX(av[nvideo]+2);
				break;
			case 'X':
				gpx = true;
				break;
			case 'K':
				kml = true;
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

	if(nvideo == ac){
		puts("*F* No Video provided");
		exit(EXIT_FAILURE);
	}

		/* Determine target directory name from the 1st video */
	int len = strlen(av[nvideo]) + 4;
	if( strlen("img0123456.png") > len )
		len = strlen("img0123456.png");
	len += strlen(av[nvideo]) + 1;
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


		/****
		 * Read videos
		 ****/
	uint32_t fr_num1=0, fr_dem1;	/* First videos informations */
	uint8_t vidx;

	for(vidx = 0; nvideo+vidx < ac; vidx++){
		newVideo();
		
			/* Open and validate the file */
		if(verbose)
			printf("*I* Reading '%s'\n", av[nvideo+vidx]);

		size_t mp4handle = OpenMP4Source(av[nvideo+vidx], MOV_GPMF_TRAK_TYPE, MOV_GPMF_TRAK_SUBTYPE, 0);
		if(!mp4handle){
			printf("*F* '%s' is an invalid MP4/MOV or it has no GPMF data\n\n", av[nvideo+vidx]);
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

		if(!fr_num1){
			fr_num1 = fr_num;
			fr_dem1 = fr_dem;
		} else if(fr_num1 != fr_num || fr_dem1 != fr_dem){
			puts("*F* Not the same frame rate");
			exit(EXIT_FAILURE);
		}


			/* Reading data */
		uint32_t index, payloads = GetNumberPayloads(mp4handle);
		if(debug)
			printf("*d* payloads : %u\n", payloads);

		for(index = 0; index < payloads; index++){
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

	if(stracker)
		GenerateAllSpeedTrkGfx( targetDir, targetFile );

	if(path)
		GenerateAllPathGfx( targetDir, targetFile );

	if(gpx)
		generateGPX( targetDir, targetFile, basename(av[nvideo]) );

	if(kml)
		generateKML( targetDir, targetFile, basename(av[nvideo]) );

	if(verbose)
		puts("");
}

