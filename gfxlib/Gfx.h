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

#include <cstdio>

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

		/* Generate only one gfx
		 * -> dir, file : as GenerateAllGfx
		 * -> index : index of the current sample
		 * -> current : The data being drawn
		 */
	void generateOneGfx(const char *dir, char *file, int index, struct GPMFdata *current);

public:
	Gfx( size_t x, size_t y, GPVideo &v ) : video(v), SX(x), SY(y), background(NULL) {};
	~Gfx(){
puts("clean");
		if(this->background)
			cairo_surface_destroy(this->background);
puts("ok");
	}

		/* Generate sticker's video
		 * -> dir : where to store the result
		 * -> file : pointer where to put the file name (part of dir)
		 *
		 * Notez-bien : GenerateAllGfx() is not called inside the constructor
		 * to let some change to modify scales if needed and/or target directory
		 */
	virtual void GenerateAllGfx( const char *dir, char *file ) = 0;
};

#endif
