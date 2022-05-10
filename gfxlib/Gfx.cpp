/* Gfx
 *
 * mother class for all graphics generation
 */

#include "Gfx.h"

#include <cstdio>
#include <cstdlib>

void Gfx::generateBackGround( void ){
	this->background = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, this->SX, this->SY);
    if(cairo_surface_status(background) != CAIRO_STATUS_SUCCESS){
        puts("*F* Can't create Cairo's surface");
        exit(EXIT_FAILURE);
    }
}
