/* Gfx.h
 *
 * mother class for all graphics generation
 */

#ifndef GFX_H
#define GFX_H

#include "../datalib/Context.h"
#include "../datalib/GPVideo.h"

#include <cstddef>
#include <cairo.h>

class Gfx {
protected :
	GPVideo &video;	// source video

	size_t SX, SY;	// size of the generated graphics
	cairo_surface_t *background;	// Background images

		/* Calculate scales to apply to convert
		 * data to point on the graphics
		 */
	virtual void calcScales( void ) = 0;

		/* Generate background image.
		 * It is generated once and reused in every image
		 */
	virtual void generateBackGround( void );

public:
	Gfx( size_t x, size_t y, GPVideo &v ) : video(v), SX(x), SY(y) {};

		/* Generate sticker's video
		 * -> dir : where to store the result
		 * -> video : GoPro video to get telemetry from
		 *
		 * Notez-bien : GenerateAllGfx() is not called inside the constructor
		 * to let some change to modify scales if needed and/or target directory
		 */
	virtual void GenerateAllGfx( const char *dir ) = 0;
};

#endif
