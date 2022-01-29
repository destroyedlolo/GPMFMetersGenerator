/* GPMFdata
 * In application data storage.
 *
 */

#include "GPMFdata.h"

#include <stddef.h>
#include <stdio.h>

	/* Minimum and maximum data */
struct GPMFdata min, max;

	/* Pointers to dynamic data */
struct GPMFdata *first=NULL, *last=NULL;

double nextsample=-1;

	/* Add a new sample
	 * Min and Max are always took in account
	 * The sample is stored only if its took at least SAMPLE seconds after the
	 * last stored sample.
	 *
	 * -> false if a gap is detected
	 */
bool addSample( double sec, double lat, double lgt, double alt, double s2d, double s3d ){
	double ret=0;

		/* Min / Max */
	if(nextsample < 0){	/* First data */
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
	if(nextsample < 0 || sec >= nextsample){
		if(nextsample >= 0 && sec > nextsample + SAMPLE/2)	/* Drifting */
			ret = sec - (nextsample + SAMPLE/4);

		nextsample = sec + SAMPLE;
		
		if(debug)
			printf("accepted : %f, next:%f\n", sec, nextsample);
	}

	return ret;
}
