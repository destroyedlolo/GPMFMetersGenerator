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

	/* Draw GPMF data
	 * -> offset : move the curve (used to draw the shadow)
	 * -> current : current position.
	 *  if set, the color will not be the same in front and in back of this position
	 *  if not set, we are drawing the shadow : THE PATH IS PRESERVED
	 */
	void drawGPMF(cairo_t *cr, int offset, GPVideo::GPMFdata *current);

protected:
	void calcScales( void );
	void generateBackground( GPX * );
	void generateOneGfx( const char *, char *, int , GPVideo::GPMFdata *, GPX * );

public:
	AltitudeGfx(GPVideo &v);

	void GenerateAllGfx( const char *dir, char *file, GPX * );
};

#endif
