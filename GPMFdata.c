/* GPMFdata
 * In application data storage.
 *
 */

#include "Shared.h"
#include "GPMFdata.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

	/* Minimum and maximum data */
struct GPMFdata min, max;

	/* Pointers to dynamic data */
struct GPMFdata *first = NULL, *last = NULL;
uint32_t samples_count = 0;

static double nextsample=0;	/* Next sample timestamp to consider */
static double voffset=0;	/* Video time offset */
static double lastTS;		/* Last sample timestamp */

	/* New video
	 * Reset video offset
	 */
void newVideo( void ){
	voffset += lastTS;
}

	/* Add a new sample
	 * Min and Max are always took in account
	 * The sample is stored only if its took at least SAMPLE seconds after the
	 * last stored sample.
	 */
double addSample( double sec, double lat, double lgt, double alt, double s2d, double s3d ){
	double ret=0;

	s2d *= 3.6;
	s3d *= 3.6;

		/* Min / Max */
	if(!first){	/* First data */
		min.latitude = max.latitude = lat;
		min.longitude = max.longitude = lgt;
		min.altitude = max.altitude = alt;
		min.spd2d = max.spd2d = s2d;
		min.spd3d = max.spd3d = s3d;
	} else {
		if(lat < min.latitude)
			min.latitude = lat;
		if(lat > max.latitude)
			max.latitude = lat;

		if(lgt < min.longitude)
			min.longitude = lgt;
		if(lgt > max.longitude)
			max.longitude = lgt;

		if(alt < min.altitude)
			min.altitude = alt;
		if(alt > max.altitude)
			max.altitude = alt;

		if(s2d < min.spd2d)
			min.spd2d = s2d;
		if(s2d > max.spd2d)
			max.spd2d = s2d;

		if(s3d < min.spd3d)
			min.spd3d = s3d;
		if(s3d > max.spd3d)
			max.spd3d = s3d;
	}

		/* Store sample if needed */
	lastTS = sec;
	sec += voffset;
	if(!first || sec >= nextsample){
		if(first && sec > nextsample + SAMPLE/2)	/* Drifting */
			ret = sec - (nextsample + SAMPLE/2);

		nextsample += SAMPLE;
		if(debug)
			printf("accepted : %f, next:%f\n", sec, nextsample);

		struct GPMFdata *nv = malloc( sizeof(struct GPMFdata) );
		if(!nv){
			puts("*F* out of memory");
			exit(EXIT_FAILURE);
		}

			/* Insert the new sample */
		if(!first)
			first = nv;
		else
			last->next = nv;

		nv->next = NULL;
		nv->latitude = lat;
		nv->longitude = lgt;
		nv->altitude = alt;
		nv->spd2d = s2d;
		nv->spd3d = s3d;
	
		samples_count++;
		last = nv;
	}

	return ret;
}

void dumpSample( void ){
	puts("*I* Video min/max :");
	printf("\tLatitude : %f deg - %f deg (%f)\n", min.latitude, max.latitude, max.latitude - min.latitude);
	printf("\tLongitude : %f deg - %f deg (%f)\n", min.longitude, max.longitude, max.longitude - min.longitude);
	printf("\tAltitude : %.3f m - %.3f m (%.3f)\n", min.altitude, max.altitude, max.altitude - min.altitude);
	printf("\tSpeed2d : %.3f km/h - %.3f km/h (%.3f)\n", min.spd2d, max.spd2d, max.spd2d - min.spd2d);
	printf("\tSpeed3d : %.3f km/h - %.3f km/h (%.3f)\n", min.spd3d, max.spd3d, max.spd3d - min.spd3d);

	if(debug){
		struct GPMFdata *p;

		puts("*D* Memorized video data");
		for(p = first; p; p = p->next){
			printf("%p (next: %p)\n", p, p->next);
			printf("\tLatitude : %.3f deg\n", p->latitude);
			printf("\tLongitude : %.3f deg\n", p->longitude);
			printf("\tAltitude : %.3f m\n", p->altitude);
			printf("\tSpeed2d : %.3f km/h\n", p->spd2d);
			printf("\tSpeed3d : %.3f km/h\n", p->spd3d);
		}
	}

	printf("*I* %u memorised samples\n", samples_count);
}

