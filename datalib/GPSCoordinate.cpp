#include "GPSCoordinate.h"

std::string GPSCoordinate::strLocalTime( void ){
	char buf[26];
	strftime(buf, sizeof(buf), "%FT%T%z", this->getLocalTime());

	return std::string(buf);
}

std::string GPSCoordinate::strLocalHour( bool HM_only ){
	char buf[9];
	strftime(buf, sizeof(buf), HM_only ? "%R" : "%T", this->getLocalTime());

	return std::string(buf);
}

double GPSCoordinate::Estrangement( GPSCoordinate &other ){
		double a = this->toRadian(other.getLatitude());
		double b = this->toRadian(this->getLatitude());
		double c = this->toRadian(other.getLongitude());
		double d = this->toRadian(this->getLongitude());

		return( R*acos(sin(a)*sin(b) + cos(a)*cos(b)*cos(c-d)) );
}
