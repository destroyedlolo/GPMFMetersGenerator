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
	double cumulative_distance;

public :
	GPSCoordinate(){}
	GPSCoordinate( double alat, double along, double alt, time_t ast): 
		latitude(alat), longitude(along), altitude(alt),
		sample_time(ast), cumulative_distance(0){}

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

	double addDistance( GPSCoordinate &other ){
		return( this->cumulative_distance = this->Estrangement(other) + other.getCumulativeDistance() );
	}

	double getLatitude( void ){ return this->latitude; }
	double getLongitude( void ){ return this->longitude; }
	double getAltitude( void ){ return this->altitude; }
	time_t getSampleTime( void ){ return this->sample_time; }
	double getCumulativeDistance( void ){ return this->cumulative_distance; }

	struct tm *getGMT( void ){ return gmtime(&this->sample_time); }
	struct tm *getLocalTime( void ){ return localtime(&this->sample_time); }
	std::string strLocalTime( void );
	std::string strLocalHour( bool HM_only = false );

		/* from https://forums.futura-sciences.com/mathematiques-superieur/306536-calcul-de-distance-entre-2-points-dont-jai-coordonnees-geographiques-longitude-latitude.html#post2315609 */
	double Estrangement( GPSCoordinate &other );

	static double toRadian( double deg ){ return(deg * M_PI/180.0); }
};

#endif
