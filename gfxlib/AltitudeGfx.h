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

		/* Story's overwriting */
	int soffx;			// Video's own offset
	double sscale_w;	// Video sample to GPX ones

	bool forcegpx;		// Use GPX for drawing the curve

	/* Draw external GPX data
	 * cr -> cairo context
	 * offset -> offset the curve (do draw shadows)
	 */
	void drawGPX(cairo_t *cr, int offset);

	/* Draw GPMF data
	 * -> offset : move the curve (used to draw the shadow)
	 * -> current : current position.
	 *  if set, the color will not be the same in front and in back of this position
	 *  if not set, we are drawing the shadow : THE PATH IS PRESERVED
	 */
	void drawGPMF(cairo_t *cr, int offset, uint32_t current=-1);

protected:
	void calcScales( void );
	void generateBackground( void );
	void generateOneGfx( const char *, char *, int , GPMFdata & );

public:
	AltitudeGfx(GPVideo &v, GPX *h, bool forcegpx);

	void GenerateAllGfx( const char *dir, char *file );
};

#endif
