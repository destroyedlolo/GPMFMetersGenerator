/* AltitudeGraphic
 * Generate altitude graphics
 */
#include "AltitudeGfx.h"

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>

AltitudeGfx::AltitudeGfx(GPVideo &v) : Gfx( 600,300, v ) {
	this->calcScales();
}

void AltitudeGfx::calcScales( void ){
	if(!this->background)
		Gfx::generateBackGround();

		/* Label */
	cairo_text_extents_t extents;
	cairo_t *cr = cairo_create(this->background);

	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 35);
	cairo_text_extents(cr, "8888", &extents);
	this->offy = extents.height + 5;
	this->posLabel = this->SX - extents.x_advance - 55;

	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 27);
	cairo_text_extents(cr, "8888", &extents);
	this->offx = extents.x_advance + 10;

		/* Scales */
	this->min_h = (((int)this->video.getMin().altitude)/50)*50;
	this->max_h = (((int)this->video.getMax().altitude)/50 + 1)*50;


	this->range_h = this->max_h - this->min_h;
	this->scale_h = (double)(this->SY - offy)/(double)this->range_h;
	this->scale_w = (double)(this->SX - offx)/(double)video.getSampleCount();
	this->delta_h = ((this->range_h/5)/50)*50;
	if(!this->delta_h)
		this->delta_h = this->range_h/5;

	cairo_destroy(cr);
}

void AltitudeGfx::drawGPMF(cairo_t *cr, int offset, struct GPMFdata *current){
	if(!current){	/* Drawing shadow */
		cairo_set_source_rgba(cr, 0,0,0, 0.55);
		cairo_set_line_width(cr, 5);
	} else {	/* Drawing curve */
		cairo_set_line_width(cr, 3);
		cairo_set_source_rgb(cr, 0.11, 0.65, 0.88);	/* Set white color */
	}

	struct GPMFdata *p;
	uint32_t i;
	for(i = 0, p = this->video.getFirst(); i < video.getSampleCount(); i++, p=p->next){
		int x = this->offx + i*this->scale_w + offset;
		int y = this->SY - (p->altitude - this->min_h)*this->scale_h + offset;

		if(!i)	/* First plot */
			cairo_move_to(cr, x, y);
		else
			cairo_line_to(cr, x, y);

		if(current == p){
			cairo_stroke(cr);
			cairo_move_to(cr, x, y);
			cairo_set_source_rgb(cr, 1,1,1);
		}
	}

	if(current)
		cairo_stroke(cr);
	else
		cairo_stroke_preserve(cr);
}

void AltitudeGfx::generateBackGround( ){
	if(!this->background)
		Gfx::generateBackGround();

	cairo_t *cr = cairo_create(this->background);

	cairo_set_source_rgba(cr, 1,1,1, 0.80);	/* Set white color */
	cairo_set_line_width(cr, 1);

	char t[8];
	for(int i = this->min_h; i <= this->max_h; i += this->delta_h){
		int y = this->SY - (i-this->min_h)*this->scale_h;
		sprintf(t, "%5d", i);

		cairo_move_to(cr, 3, y);
		cairo_show_text(cr, t);
		cairo_move_to(cr, this->offx, y);
		cairo_line_to(cr, this->SX, y);
	}
	cairo_stroke(cr);

	drawGPMF(cr, 3, NULL);	// Draw Shadow
	cairo_pattern_t *pat = cairo_pattern_create_linear(this->offx,this->SY - this->range_h*this->scale_h, this->offx,this->SY);
	cairo_pattern_add_color_stop_rgba(pat, 0, 0,0,0, 0.25);
	cairo_pattern_add_color_stop_rgba(pat, 1, 0,0,0, 0.05);
	cairo_set_source(cr, pat);

	cairo_line_to(cr, this->SX, this->SY);
	cairo_line_to(cr, this->offx, this->SY);
	cairo_line_to(cr, this->offx, this->SY - (this->video.getFirst()->altitude - this->min_h)*this->scale_h);

	cairo_fill(cr);

		/* Cleaning */
	cairo_pattern_destroy(pat);
	cairo_destroy(cr);

}

void AltitudeGfx::generateOneGfx(const char *fulltarget, char *filename, int index, struct GPMFdata *current){
}

void AltitudeGfx::GenerateAllGfx( const char *fulltarget, char *filename ){
	this->generateBackGround();
}

