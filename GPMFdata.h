/* GPMFdata
 * In application data storage.
 *
 * GPMF raw data :
 * 	Latitude : degree
 * 	Longitude : degree
 * 	Altitude : meter
 * 	2d Speed : m/s
 * 	3d Speed : m/s
 *
 * The frequency of reception of GPS data (18.18...Hz) causes a temporal drift of 
 * data arrival. Since it is not useful to have so much data, and to facilitate post
 * processing, only 8 samples / seconds are kept. 
 */

#ifndef GPMFDATA_H
#define GPMFDATA_H

#include <stdbool.h>

struct GPMFdata {
	struct GPMFdata *next;
	double latitude, longitude;
	double altitude;
	double spd2d;
	double spd3d;
};

extern struct GPMFdata min, max;
extern struct GPMFdata *first, *last;

	/* Delay b/w sample */
#define SAMPLE 1.0/8.0

extern bool addSample( double sec, double lat, double lgt, double alt, double s2d, double s3d );

#endif
