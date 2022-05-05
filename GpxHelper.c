/* GpxHelper.c
 *
 * Handle external GPX data
 *
 * Quick and dirty : so no sanity check of the GPX file, we are only looking for
 * <trkpt ...> tags and don't check it is enclosed in <gpx ...> and <trk> tags
 */

#define _XOPEN_SOURCE 	/* for strptime() */

#include "Shared.h"
#include "GpxHelper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define BUFFLEN	1024

struct GpxData minGpx, maxGpx;
struct GpxData *firstGpx = NULL, *Gpx = NULL;
uint32_t Gpx_count = 0;

char buff[BUFFLEN];
char *pbuff;	/* Pointer inside the buffer */

	/* clear the buffer */
static void clear(){
	pbuff = buff;
	*pbuff = 0;
}

	/* skip all char up incomming char (mostly for '<' and '>')
	 *
	 * <- false in case of error or end of file
	 */
static bool skipTocChar(FILE *f, char c){
	while((*pbuff = fgetc(f)) != c){
		if(feof(f))
			return false;
	}

	pbuff++;
	return true;
}

	/* read and store characters up to c
	 *
	 * <- false : end of file reached
	 */
static bool readUptoChar(FILE *f, char c){
	while((*(pbuff++) = fgetc(f)) !=c){
		if(feof(f))
			return false;
		if(pbuff - buff >= BUFFLEN){
			puts("*F* string too long");
			exit(EXIT_FAILURE);
		}
	}

	*pbuff = 0;
	return true;
}

	/* Look for "string"
	 *
	 * <- false : end of file
	 */
static bool lookFor(FILE *f, const char *string){
	while(true){
		const char *s = string;

		clear();

		if(!skipTocChar(f,*string))	/* Looks for the 1st char */
			return false;

		for(s = string+1; *s; s++)	/* reads remaining */
			if((*(pbuff++) = fgetc(f)) != *s){
				if(feof(f))
					return false;
				else
					break;
			}

		if(!*s)	/* string found */
			break;
	}

	*pbuff = 0;
	return true;
}

void loadGPX(const char *file){
	FILE *f;
	double lat, lon, alt;

	if(!(f = fopen(file, "r"))){
		perror(file);
		exit(EXIT_FAILURE);
	}

	while(true){
			/* Reading track point */
		if(!lookFor(f,"<trkpt "))	/* End of file reached */
			break;

		if(!readUptoChar(f, '>'))
			break;

		if(sscanf(buff, "<trkpt lat=\"%lf\" lon=\"%lf\">", &lat, &lon) != 2)
			puts("*E* can't read values");
		else {
			struct GpxData *nv = malloc(sizeof(struct GpxData ));
			if(!nv){
				puts("*F* out of memory");
				exit(EXIT_FAILURE);
			}

			if(!firstGpx){
				minGpx.latitude = maxGpx.latitude = lat;
				minGpx.longitude = maxGpx.longitude = lon;

				firstGpx = nv;
			} else {
				if(lat < minGpx.latitude)
					minGpx.latitude = lat;
				if(lat > maxGpx.latitude)
					maxGpx.latitude = lat;
	
				if(lon < minGpx.longitude)
					minGpx.longitude = lon;
				if(lon > maxGpx.longitude)
					maxGpx.longitude = lon;
			}

			nv->latitude = lat;
			nv->longitude = lon;
			nv->next = Gpx;

			Gpx = nv;
		}

			/* Read elevation */
		clear();

		if(!lookFor(f,"<ele>")){
			puts("*F* No elevation");
			exit(EXIT_FAILURE);
		}

		if(!readUptoChar(f, '<')){	/* read up to closing tag */
			puts("*F* malformed elevation");
			exit(EXIT_FAILURE);
		}

		if(sscanf(buff, "<ele>%lf<", &alt) != 1)
			puts("*E* can't read altitude values");

		Gpx->altitude = alt;

		if(firstGpx == Gpx)
			minGpx.altitude = maxGpx.altitude = alt;
		else {
			if(alt < minGpx.altitude)
				minGpx.altitude = alt;
			if(alt > maxGpx.altitude)
				maxGpx.altitude = alt;
		}

			/* Read timestamp */
		clear();
	
		if(!lookFor(f,"<time>")){
			puts("*F* No time");
			exit(EXIT_FAILURE);
		}
	
		if(!readUptoChar(f, '<')){	/* read up to closing tag */
			puts("*F* malformed time");
			exit(EXIT_FAILURE);
		}

		struct tm t;
		memset(&t, 0, sizeof(struct tm));

		if(!strptime(buff, "<time>%FT%T%z", &t)){
			puts("*E* can't read time");
			exit(EXIT_FAILURE);
		}
	
		long int offset = t.__tm_gmtoff;
		if(debug){
			printtm(&t);
			puts("");
		}

		Gpx->time = mktime( &t );

			/* Add the offset took from the sample
			 * minus the one from the current TZ.
			 * t.__tm_gmtoff has been updated by mktime( &t );
			 */
		Gpx->time -= offset - t.__tm_gmtoff;

		Gpx_count++;
	}


	fclose(f);

#if 0
	for( struct GpxData *p = Gpx; p; p = p->next )
		printf("%p -> (%f %f)\n", p, p->latitude, p->longitude);
#endif
	puts("*I* GPX min/max:");
	printf("\tlatitude : %f -> %f (%f)\n", minGpx.latitude, maxGpx.latitude, maxGpx.latitude - minGpx.latitude);
	printf("\tlongitude : %f -> %f (%f)\n", minGpx.longitude, maxGpx.longitude, maxGpx.longitude - minGpx.longitude);
	printf("\taltitude: %f -> %f (%f)\n", minGpx.altitude, maxGpx.altitude, maxGpx.altitude - minGpx.altitude);

	struct tm *t = gmtime(&firstGpx->time);
	printf("\tTime : ");
	printtm(t);
	t = gmtime(&Gpx->time);
	printf(" -> ");
	printtm(t);
	puts("");

	printf("*I* %u memorised GPX\n", Gpx_count);
}
