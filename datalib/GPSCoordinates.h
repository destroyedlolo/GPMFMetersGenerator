/* GPSCoordinates
 */

#ifndef GPSCOORD_H
#define GPSCOORD_H

#include <cmath>

#define R (6371000)	// terrestrial radius

class GPSCoordinates {
	double latitude, longitude;

public :
	GPSCoordinates(){}
	GPSCoordinates( double alat, double along ): latitude(alat), longitude(along) {}

	void set(double alat, double along) {
		this->latitude = alat;
		this->longitude = along;
	}
	void setLatitude( double alat ){ this->latitude = alat; }
	void setLongitude( double along ){ this->longitude = along; }

	double getLatitude( void ){ return this->latitude; }
	double getLongitude( void ){ return this->longitude; }

		/* from https://forums.futura-sciences.com/mathematiques-superieur/306536-calcul-de-distance-entre-2-points-dont-jai-coordonnees-geographiques-longitude-latitude.html#post2315609 */
	double Distance( GPSCoordinates &other ){
	}

	static double toRadian( double deg ){ return(deg * M_PI/180.0); }
};

#endif
