/* GPX
 * 	Impersonates GPX data
 */

#ifndef GPX_H
#define GPX_H

#include "GPSCoordinate.h"
#include "samplesCollection.h"

#include <ctime>

struct GpxData : public GPSCoordinate {
	double altitude;
	time_t sample_time;

	GpxData(){};
	GpxData(
		double alatitude, double alongitude,
		double aaltitude,
		time_t asample_time
	) : GPSCoordinate(alatitude, alongitude),
		altitude(aaltitude), sample_time(asample_time) {
	}
};

class GPX : public samplesCollection<GpxData> {

public:
	GPX( const char * );

	void Dump( void );

		/* Check if given coordinate is close to the footprint of the hiking.
		 * It's a very quick and dirty test ... but enough to check a video
		 * has been took in the same area
		 * -> proximity_threshold in degree
		 */
	bool sameArea( GPSCoordinate &, uint32_t proximity_threshold = 0 );
};

#endif

