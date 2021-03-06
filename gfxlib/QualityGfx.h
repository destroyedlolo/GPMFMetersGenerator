/* QualityGaphic
 * Generate quality gaphic
 */

#ifndef QUALITYGFX_H
#define QUALITYGFX_H

#include "Gfx.h"

class QualityGfx : public Gfx {
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
	void drawGPMF(cairo_t *cr, int offset, uint32_t current=-1);

protected:
	void calcScales( void );
	void generateBackground( void );
	void generateOneGfx( const char *, char *, int , GPMFdata & );

	double linearcomponent( uint16_t val, uint16_t u1, double v1, uint16_t u2, double v2 );

public:
	QualityGfx(GPVideo &v, GPX *h);

	void GenerateAllGfx( const char *dir, char *file );
};
#endif
