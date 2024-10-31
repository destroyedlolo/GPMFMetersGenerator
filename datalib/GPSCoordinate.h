/* GPSCoordinate
 */

#ifndef GPSCOORD_H
#define GPSCOORD_H

#include <cmath>
#include <string>
#include <ctime>
#include "../date/include/date/date.h"

using namespace std::chrono;

#define R (6371000)	// terrestrial radius

class GPSCoordinate {
	double latitude, longitude;
	double altitude;
	time_t sample_time;
	date::sys_time<milliseconds> sample_time_ms;
	double cumulative_distance;

public :
	GPSCoordinate(){}
	GPSCoordinate( double alat, double along, double alt, time_t ast, double adst = 0): 
		latitude(alat), longitude(along), altitude(alt),
		sample_time(ast), cumulative_distance(adst){}
	GPSCoordinate( double alat, double along, double alt, time_t ast, date::sys_time<milliseconds> astm, double adst = 0): 
		latitude(alat), longitude(along), altitude(alt),
		sample_time(ast), sample_time_ms(astm), cumulative_distance(adst){}

	void set(double alat, double along, double alt, time_t ast, date::sys_time<milliseconds> astm) {
		this->latitude = alat;
		this->longitude = along;
		this->altitude = alt;
		this->sample_time = ast;
		this->sample_time_ms = astm;
	}
	void set(double alat, double along, double alt, time_t ast) {
		this->latitude = alat;
		this->longitude = along;
		this->altitude = alt;
		this->sample_time = ast;
	}
	void set( GPSCoordinate &nv ){
		this->latitude = nv.getLatitude();
		this->longitude = nv.getLongitude();
		this->altitude = nv.getAltitude();
		this->sample_time = nv.getSampleTime();
		this->sample_time_ms = nv.getSampleTimeMS();
	}

	void setLatitude( double alat ){ this->latitude = alat; }
	void setLongitude( double along ){ this->longitude = along; }
	void setAltitude( double alt ){ this->altitude = alt; }
	void setSampleTime( time_t ast ){ this->sample_time = ast; }
	void setSampleTimeMS( date::sys_time<milliseconds> astm ){ this->sample_time_ms = astm; }

	double addDistance( GPSCoordinate &other );
	double addDistance( double d ){
		return( this->cumulative_distance += d );
	}

	double getLatitude( void ){ return this->latitude; }
	double getLongitude( void ){ return this->longitude; }
	double getAltitude( void ){ return this->altitude; }
	time_t getSampleTime( void ){ return this->sample_time; }
	date::sys_time<milliseconds> getSampleTimeMS( void ){ return this->sample_time_ms; }
	double getCumulativeDistance( void ){ return this->cumulative_distance; }

	struct tm *getGMT( void ){ return gmtime(&this->sample_time); }
	struct tm *getLocalTime( void ){ return localtime(&this->sample_time); }
	std::string strLocalTime( void );
	std::string strLocalHour( bool HM_only = false );
	std::string diffTime( time_t begin, bool sec = false );	// return the duration begin -> this
	double diffTimeF( time_t begin );

		/* from https://forums.futura-sciences.com/mathematiques-superieur/306536-calcul-de-distance-entre-2-points-dont-jai-coordonnees-geographiques-longitude-latitude.html#post2315609 */
	double Estrangement( GPSCoordinate &other );

	static double toRadian( double deg ){ return(deg * M_PI/180.0); }
};

#endif
