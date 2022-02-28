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
 * processing, only 9 samples / seconds are kept. 
 */

#ifndef GPMFDATA_H
#define GPMFDATA_H

#include <stdint.h>

struct GPMFdata {
	struct GPMFdata *next;
	double latitude, longitude;
	double altitude;
	double spd2d;
	double spd3d;
};

extern struct GPMFdata min, max;
extern struct GPMFdata *first, *last;
extern uint32_t samples_count;	/* how many samples */

	/* Delay b/w sample */
#define SAMPLE (1.0/9.0)


	/* We are starting a new video
	 */
extern void newVideo( void );

	/* Add a new sample
	 * -> sec : timestamp
	 * -> ... : data to store
	 *
	 * <- not 0 if drift detected
	 */
extern double addSample( double sec, double lat, double lgt, double alt, double s2d, double s3d );

	/* Dump stored data 
	 * relies on global verbose and debug
	 */
extern void dumpSample( void );
#endif
