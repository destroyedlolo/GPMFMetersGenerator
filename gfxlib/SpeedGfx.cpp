/* SpeedGraphic
 * Generate speed-o-meter graphics
 *
 */

#include "SpeedGfx.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

#define LABEL_SZ 55	/* Label font size */

SpeedGfx::SpeedGfx(GPVideo &v, char atype) : Gfx( 300,300, v ), type(atype) {
	this->calcScales();
}

double SpeedGfx::transforme( double angle ){
	return -(1.5 * M_PI - angle);
}

void SpeedGfx::calcScales( void ){
		/* Scale
		 * As of speed, graphics's minimal is always 0
		 */
	if(this->type == 'b'){
		this->range = ((int)(this->video.getMax().spd3d/10) + 1)*10;
		int range2 = ((int)(this->video.getMax().spd2d/10) + 1)*10;
		if(range2 > this->range)
			this->range = range2;
	} else
		this->range = (((int)((type == '3') ? this->video.getMax().spd3d : this->video.getMax().spd2d))/10 + 1)*10;
	this->scale = 3.0/2.0* M_PI/(double)this->range;

		/* Label's offset */
	cairo_t *cr = cairo_create(this->background);

	cairo_text_extents_t extents;
	int speed = (int)(log10(range)) + 1;
	char t[11], *p = t;
	for(;speed; speed--)
		*(p++) = '8';
	strcpy(p, ".8");
	
	cairo_set_font_size(cr, LABEL_SZ);
	cairo_text_extents(cr, t, &extents);
	
	this->offlabel = (this->SX - extents.x_advance)/2;

	cairo_destroy(cr);
}

void SpeedGfx::GenerateAllGfx( const char *dir ){
}
