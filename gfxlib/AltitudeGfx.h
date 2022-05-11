/* AltitudeGraphic
 * Generate altitude graphics
 */
#ifndef ALTITUDEGFX_H
#define ALTITUDEGFX_H

#include "Gfx.h"

class AltitudeGfx : public Gfx {
	int posLabel;		// Label offset
	int offx, offy;		// Graphic's offset

	int min_h, max_h, range_h;	// Range values
	int delta_h;
	double scale_h, scale_w;	// Scales

protected:
	void calcScales( void );
	void generateBackGround( void );
	void generateOneGfx(const char *, char *, int , struct GPMFdata *);

public:
	AltitudeGfx(GPVideo &v);

	void GenerateAllGfx( const char *dir, char *file );
};

#endif
