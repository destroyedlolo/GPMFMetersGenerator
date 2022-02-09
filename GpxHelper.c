/* GpxHelper.c
 *
 * Handle external GPX data
 *
 * Quick and dirty : so no sanity check of the GPX file, we are only looking for
 * <trkpt ...> tags and don't check it is enclosed in <gpx ...> and <trk> tags
 */

#include "GpxHelper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define BUFFLEN	1024

struct GpxData minGpx, maxGpx;
struct GpxData *firstGpx = NULL, *Gpx = NULL;

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
	double lat, lon;

	if(!(f = fopen(file, "r"))){
		perror(file);
		exit(EXIT_FAILURE);
	}

	while(true){
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
			}

			nv->latitude = lat;
			nv->longitude = lon;
			nv->next = Gpx;

			Gpx = nv;
		}
	}


	fclose(f);
}
