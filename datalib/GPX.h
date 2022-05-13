/* GPX
 * 	Impersonates GPX data
 */

#ifndef GPX_H
#define GPX_H

#include <cstdint>
#include <ctime>

class GPX {
public:
	struct GpxData {
		struct GpxData *next;
		double latitude, longitude;
		double altitude;
		time_t sample_time;

		GpxData(){};
		GpxData(
			double alatitude, double alongitude,
			double aaltitude,
			time_t asample_time
		) : next(NULL), latitude(alatitude), longitude(alongitude),
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
};

#endif

