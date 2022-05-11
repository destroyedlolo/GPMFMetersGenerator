/* PathGraphic
 * Generate path graphics
 */
#ifndef ALTITUDEGFX_H
#define ALTITUDEGFX_H

#include "Gfx.h"

class PathGfx : public Gfx {

	/* Draw GPMF data
	 * -> offset : move the curve (used to draw the shadow)
	 * -> current : current position.
	 *  if set, the color will not be the same in front and in back of this position
	 *  if not set, we are drawing the shadow : THE PATH IS PRESERVED
	 */
	void drawGPMF(cairo_t *cr, int offset, struct GPMFdata *current);

protected:
	void calcScales( void );
	void generateBackGround( void );
	void generateOneGfx(const char *, char *, int , struct GPMFdata *);

public:
	PathGfx(GPVideo &v);

	void GenerateAllGfx( const char *dir, char *file );
};

#endif
