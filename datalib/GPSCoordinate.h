/* GPSCoordinate
 */

#ifndef GPSCOORD_H
#define GPSCOORD_H

#include <cmath>
#include <string>

#define R (6371000)	// terrestrial radius

class GPSCoordinate {
	double latitude, longitude;
	double altitude;
	time_t sample_time;

public :
	GPSCoordinate(){}
	GPSCoordinate( double alat, double along, double alt, time_t ast): 
		latitude(alat), longitude(along), altitude(alt), sample_time(ast){}

	void set(double alat, double along, double alt, time_t ast) {
		this->latitude = alat;
		this->longitude = along;
		this->altitude = alt;
		this->sample_time = ast;
	}
	void setLatitude( double alat ){ this->latitude = alat; }
	void setLongitude( double along ){ this->longitude = along; }
	void setAltitude( double alt ){ this->altitude = alt; }
	void setSampleTime( time_t ast ){ this->sample_time = ast; }

	double getLatitude( void ){ return this->latitude; }
	double getLongitude( void ){ return this->longitude; }
	double getAltitude( void ){ return this->altitude; }
	time_t getSampleTime( void ){ return this->sample_time; }

	struct tm *getGMT( void ){ return gmtime(&this->sample_time); }
	struct tm *getLocalTime( void ){ return localtime(&this->sample_time); }
	std::string strLocalTime( void );
	std::string strLocalHour( bool HM_only = false );

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
