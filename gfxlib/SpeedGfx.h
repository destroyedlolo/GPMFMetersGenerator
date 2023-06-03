/* SpeedGraphic
 * Generate speed-o-meter graphics
 *
 */
#ifndef SPEEDGFX_H
#define SPEEDGFX_H

#include "Gfx.h"

class SpeedGfx : public Gfx {
	int range;		// data range
	double scale;	// computed scale data->gfx
	int offlabel;	// offset to display the label
	int offgrade;	// offset for the grade label
	char type;		// type of graphics to display

	double transforme( double angle );

protected:
	void calcScales( void );
	void generateBackground( void );
	void generateOneGfx( const char *, char *, int , GPMFdata & ){};
	void generateOneGfx( const char *fulltarget, char *filename, int index, GPMFdata &current, int prc );
	
public:
	SpeedGfx(GPVideo &v, GPX *h, char atype='2');

	void GenerateAllGfx( const char *dir, char *file );
};

#endif
