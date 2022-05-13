/* GPVideo
 * 	Impersonates GoPro video
 */

#ifndef GPVIDEO_H
#define GPVIDEO_H

#include <cstdint>
#include <ctime>

	/* Number of samples per seconds (9) */
#define SAMPLE (1.0/9.0)

class GPVideo {
public:
		/* Sample's data */
	struct GPMFdata {
		struct GPMFdata *next;
		double latitude, longitude;
		double altitude;
		double spd2d, spd3d;
		time_t sample_time;

		GPMFdata(){};
		GPMFdata( 
			double alatitude, double alongitude,
			double aaltitude,
			double aspd2d, double aspd3d,
			time_t asample_time
		) : next(NULL), latitude(alatitude), longitude(alongitude),
			altitude(aaltitude), spd2d(aspd2d), spd3d(aspd3d), 
			sample_time(asample_time){
		}
	};

private:

		/* video's */
	size_t mp4handle;
	uint32_t fr_num, fr_dem;	// Video framerates

		/* GPMF's */
	struct GPMFdata min, max;
	struct GPMFdata *first, *last;
	double nextsample;			// timing of the next sample to store
	uint32_t samples_count;		// number of samples stored

		/* Multiparts' */
	double voffset;		// part's timing offset
	double lastTiming;	// last timing

protected:
	void readGPMF( void );

	/* Add a new sample
	 * Min and Max are always took in account
	 * The sample is stored only if its took at least SAMPLE seconds after the
	 * last stored sample.
	 */
	double addSample( double sec, double lat, double lgt, double alt, double s2d, double s3d, time_t time );

public:
		/* Read and parse 1st video */
	GPVideo( char * );

		/* Read and parse another part */
	void AddPart( char * );

	void Dump( void );

	struct GPMFdata &getMin( void ){ return this->min; };
	struct GPMFdata &getMax( void ){ return this->max; };
	struct GPMFdata *getFirst(void){ return this->first; };
	struct GPMFdata *getLast(void){ return this->last; };
	uint32_t getSampleCount(void){ return this->samples_count; };
};
#endif
