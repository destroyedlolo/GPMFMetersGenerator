/* GPX
 * 	Impersonates GPX data
 */

#ifndef GPX_H
#define GPX_H

#include "GPSCoordinate.h"

#include <cstdint>
#include <ctime>

#include <vector>

class GPX {
public:
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

	std::vector<GpxData> samples;

private:
	GpxData min, max;
	uint32_t samples_count;		// number of samples stored

public:
	GPX( const char * );

	void Dump( void );

	std::vector<GpxData> &getSamples(void){ return this->samples; };
	GpxData &getMin( void ){ return this->min; };
	GpxData &getMax( void ){ return this->max; };
	GpxData &getFirst(void){ return this->samples.front(); };
	GpxData &getLast(void){ return this->samples.back(); };
	uint32_t getSampleCount(void){ return this->samples.size(); };

		/* Check if given coordinate is close to the footprint of the hiking.
		 * It's a very quick and dirty test ... but enough to check a video
		 * has been took in the same area
		 * -> proximity_threshold in degree
		 */
	bool sameArea( GPSCoordinate &, uint32_t proximity_threshold = 0 );
};

#endif

