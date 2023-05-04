/* GPVideo
 * 	Impersonates GoPro video
 */

#ifndef GPVIDEO_H
#define GPVIDEO_H

#include "GPSCoordinate.h"
#include "samplesCollection.h"

#include <ctime>

	/* default number of samples per seconds (9) */
#define SAMPLE 9

struct GPMFdata : public GPSCoordinate {
	double spd2d, spd3d;
	unsigned char gfix;		// GPS fixe
	uint16_t dop;		// GPS Dilution of Precision

	GPMFdata(){};
	GPMFdata( 
		double alatitude, double alongitude,
		double aaltitude,
		double aspd2d, double aspd3d,
		time_t asample_time,
		unsigned char agfix,
		uint16_t adop
	) : GPSCoordinate(alatitude, alongitude, aaltitude, asample_time),
		spd2d(aspd2d), spd3d(aspd3d), gfix(agfix), dop(adop){
	}
};

class GPVideo : public samplesCollection<GPMFdata> {
private:

		/* video's */
	size_t mp4handle;
	uint32_t fr_num, fr_dem;	// Video framerates

		/* GPMF's */
	double nextsample;			// timing of the next sample to store
	double sample;				// Sample rate

		/* Multiparts' */
	double voffset;		// part's timing offset
	double lastTiming;	// last timing

		/* fields to ensure each sample contains a pertinent value
		 * when stored (e.g : for dop, the maximum value b/w sample)
		 */
	uint16_t dop;

		/* Accumulation to smooth values */
	double calt;
	double cs2d, cs3d;

	unsigned int nbre;

protected:
	void readGPMF( double cumul_dst );

	/* Add a new sample
	 * Min and Max are always took in account
	 * The sample is stored only if its took at least SAMPLE seconds after the
	 * last stored sample.
	 */
	double addSample( double sec, double lat, double lgt, double alt, double s2d, double s3d, time_t time, unsigned char gfix, uint16_t dop, double cumul_dst );

public:
		/* Read and parse 1st video
		 * fch : first video sample
		 * sample : number of samples per seconds
		 * cumul_dst : initial cumulative distance
		 */
	GPVideo( char *fch, unsigned int sample=SAMPLE, double cumul_dst = 0.0);

		/* Read and parse another part */
	void AddPart( char *, double cumul_dst );

	void Dump( void );
};
#endif
