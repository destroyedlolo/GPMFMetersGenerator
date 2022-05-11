/* PathGraphic
 * Generate path graphics
 */
#include "PathGfx.h"

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <cmath>

PathGfx::PathGfx(GPVideo &v) : Gfx( 600,300, v ) {
	this->calcScales();
}

void PathGfx::calcScales( void ){
	if(!this->background)
		Gfx::generateBackGround();

}

void PathGfx::drawGPMF(cairo_t *cr, int offset, struct GPMFdata *current){
}

void PathGfx::generateBackGround( ){
}

void PathGfx::generateOneGfx(const char *fulltarget, char *filename, int index, struct GPMFdata *current){
}

void PathGfx::GenerateAllGfx( const char *fulltarget, char *filename ){
	Gfx::GenerateAllGfx( fulltarget, filename );

		/* Generate video */
	if(genvideo)
		generateVideo(fulltarget, filename, "alt", "altitude");
}
