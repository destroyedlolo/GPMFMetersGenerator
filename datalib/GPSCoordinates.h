/* GPSCoordinates
 */

#ifndef GPSCOORD_H
#define GPSCOORD_H

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
};

#endif
