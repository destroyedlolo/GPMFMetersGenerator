/* GPX
 * 	Impersonates GPX data
 */

#ifndef GPX_H
#define GPX_H

#include "GPSCoordinate.h"

#include <cstdint>
#include <ctime>

class GPX {
public:
	struct GpxData : public GPSCoordinate {
		struct GpxData *next;
		double altitude;
		time_t sample_time;

		GpxData(){};
		GpxData(
			double alatitude, double alongitude,
			double aaltitude,
			time_t asample_time
		) : GPSCoordinate(alatitude, alongitude), next(NULL),
			altitude(aaltitude), sample_time(asample_time) {
		}
	};

private:
	GpxData min, max;
	GpxData *first, *last;
	uint32_t samples_count;		// number of samples stored

public:
	GPX( const char * );

	void Dump( void );

	GpxData &getMin( void ){ return this->min; };
	GpxData &getMax( void ){ return this->max; };
	GpxData *getFirst(void){ return this->first; };
	GpxData *getLast(void){ return this->last; };
	uint32_t getSampleCount(void){ return this->samples_count; };

		/* Check if given coordinate is close to the footprint of the hiking.
		 * It's a very quick and dirty test ... but enough to check a video
		 * has been took in the same area
		 */
	bool sameArea( GPSCoordinate &, uint32_t proximity_threshold = 0 );
};

#endif

