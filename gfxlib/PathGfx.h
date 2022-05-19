/* PathGraphic
 * Generate path graphics
 */
#ifndef PATHGFX_H
#define PATHGFX_H

#include "Gfx.h"

class PathGfx : public Gfx {
	int min_x, min_y;
	int max_x, max_y;

	int range_x, range_y;
	int off_x, off_y;		// offsets
	double scale;

	/* Convert position to coordinates */
	void posXY( double, double, int &, int &);

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
	PathGfx(GPVideo &v, GPX *);

	void GenerateAllGfx( const char *dir, char *file );
};

#endif
