/* GPSCoordinate
 */

#ifndef GPSCOORD_H
#define GPSCOORD_H

#include <cmath>

#define R (6371000)	// terrestrial radius

class GPSCoordinate {
	double latitude, longitude;

public :
	GPSCoordinate(){}
	GPSCoordinate( double alat, double along ): latitude(alat), longitude(along) {}

	void set(double alat, double along) {
		this->latitude = alat;
		this->longitude = along;
	}
	void setLatitude( double alat ){ this->latitude = alat; }
	void setLongitude( double along ){ this->longitude = along; }

	double getLatitude( void ){ return this->latitude; }
	double getLongitude( void ){ return this->longitude; }

		/* from https://forums.futura-sciences.com/mathematiques-superieur/306536-calcul-de-distance-entre-2-points-dont-jai-coordonnees-geographiques-longitude-latitude.html#post2315609 */
	double Distance( GPSCoordinate &other ){
		double a = this->toRadian(other.getLatitude());
		double b = this->toRadian(this->getLatitude());
		double c = this->toRadian(other.getLongitude());
		double d = this->toRadian(this->getLongitude());

		return( R*acos(sin(a)*sin(b) + cos(a)*cos(b)*cos(c-d)) );
	}

	static double toRadian( double deg ){ return(deg * M_PI/180.0); }
};

#endif
