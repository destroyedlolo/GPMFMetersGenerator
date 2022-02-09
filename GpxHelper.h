/* GpxHelper.h
 *
 * Handle external GPX data
 */

#ifndef GPXHELPER_H
#define GPXHELPER_H

	/* Notez-bien : unlike GPMF data, GPX ones don't need to be used
	 * "in order". Consequently, 
	 * 		Gpx - is the starting point and is the last data read
	 *		firstGpx - is only used to plot the starting spot
	 */
struct GpxData {
	struct GpxData *next;
	double latitude, longitude;
};

extern struct GpxData minGpx, maxGpx;
extern struct GpxData *firstGpx, *Gpx;

extern void loadGPX(const char *file);

#endif
