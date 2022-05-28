/* QualityGaphic
 * Generate quality gaphic
 */

#include "QualityGfx.h"

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <cmath>

QualityGfx::QualityGfx(GPVideo &v, GPX *h) : Gfx( 600,150, v, h ) {
	this->calcScales();
}

void QualityGfx::calcScales( void ){
		/* Label */
	cairo_text_extents_t extents;
	cairo_t *cr = cairo_create(this->background);

	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 35);
	cairo_text_extents(cr, "8888 (8)", &extents);
	this->offy = extents.height + 5;
	this->posLabel = this->SX - extents.x_advance - 55;

	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 20);
	cairo_text_extents(cr, "8888", &extents);
	this->offx = extents.x_advance + 10;

		/* Scales */
	this->min_h = (((int)this->video.getMin().dop)/25)*25;
	this->max_h = (((int)this->video.getMax().dop)/25 + 1)*25;
	this->scale_w = (double)(this->SX - offx)/(double)this->video.getSampleCount();

	this->range_h = this->max_h - this->min_h;
	this->scale_h = (double)(this->SY - offy)/(double)this->range_h;
	this->delta_h = ((this->range_h/5)/25)*25;
	if(!this->delta_h)
		this->delta_h = this->range_h/5;

	cairo_destroy(cr);
}

void QualityGfx::drawGPMF(cairo_t *cr, int offset, uint32_t current){
	if(offset){	// drawing shadow
		cairo_set_line_width(cr, 4);
		cairo_set_source_rgba(cr, 0,0,0, 0.55);
	} else {	// drawing path
		cairo_set_source_rgb(cr, 1,1,1);
		cairo_set_line_width(cr, 2);
	}

	for(uint32_t i = 0; i < this->video.getSampleCount(); i++){
		int x = this->offx + i*this->scale_w + offset;
		int y = this->SY - (this->video[i].dop - this->min_h)*this->scale_h + offset;

		if(!i)	/* First plot */
			cairo_move_to(cr, x, y);
		else
			cairo_line_to(cr, x, y);

		if(current == i){
			cairo_stroke(cr);
			cairo_move_to(cr, x, y);
			cairo_set_source_rgb(cr, 1,1,1);
		}
	}

	if(current != uint32_t(-1))
		cairo_stroke(cr);
	else
		cairo_stroke_preserve(cr);
}

void QualityGfx::generateBackground( void ){
	cairo_t *cr = cairo_create(this->background);

	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 20);
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

	this->drawGPMF(cr, 3);

	cairo_pattern_t *pat = cairo_pattern_create_linear(this->offx,this->SY - this->range_h*this->scale_h, this->offx,this->SY);
	cairo_pattern_add_color_stop_rgba(pat, 0, 0,0,0, 0.25);
	cairo_pattern_add_color_stop_rgba(pat, 1, 0,0,0, 0.05);
	cairo_set_source(cr, pat);

	cairo_line_to(cr, this->SX, this->SY);
	cairo_line_to(cr, this->offx, this->SY);
	cairo_line_to(cr, this->offx, this->SY - (this->video.getFirst().getAltitude() - this->min_h)*this->scale_h);

	cairo_fill(cr);

	cairo_pattern_destroy(pat);
	cairo_destroy(cr);
}

void QualityGfx::generateOneGfx(const char *fulltarget, char *filename, int index, GPMFdata &current){

		/*
		 * Initialise Cairo
		 */
	cairo_status_t err;

	cairo_surface_t *srf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, this->SX, this->SY);
	if(cairo_surface_status(srf) != CAIRO_STATUS_SUCCESS){
		puts("*F* Can't create Cairo's surface");
		exit(EXIT_FAILURE);
	}

	cairo_t *cr = cairo_create(srf);

	cairo_set_source_surface(cr, this->background, 0, 0);
	cairo_rectangle(cr, 0,0, this->SX, this->SY);
	cairo_fill(cr);
	cairo_stroke(cr);

		/* Draw Altitude curve */
	this->drawGPMF(cr, 0, index);

		/* Display the label */
	char t[10];
	sprintf(t, "%5d (%c)", current.dop, current.gfix+'0');
	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 35);

	cairo_set_source_rgba(cr, 0,0,0, 0.55);
	cairo_move_to(cr, this->posLabel+2, this->offy+2);
	cairo_show_text(cr, t);
	cairo_stroke(cr);

	cairo_set_source_rgb(cr, 1,1,1);	/* Set white color */
	cairo_move_to(cr, this->posLabel, this->offy);
	cairo_show_text(cr, t);
	cairo_stroke(cr);

		/* Display the spot */
	cairo_set_line_width(cr, 5);
	cairo_arc(cr, this->offx + index*this->scale_w, this->SY - (current.dop - this->min_h)*this->scale_h , 8, 0, 2 * M_PI);
	cairo_stroke_preserve(cr);
	cairo_set_source_rgb(cr, 0.8, 0.2, 0.2);
	cairo_fill(cr);

		/* Writing the image */
	sprintf(filename, "qual%07d.png", index);
	if(verbose)
		printf("*D* Writing '%s'\r", fulltarget);

	if((err = cairo_surface_write_to_png(srf, fulltarget)) != CAIRO_STATUS_SUCCESS){
		printf("*F* Writing surface : %s / %s\n", cairo_status_to_string(err), strerror(errno));
		exit(EXIT_FAILURE);
	}

		/* Cleaning */
	cairo_destroy(cr);
	cairo_surface_destroy(srf);
}

void QualityGfx::GenerateAllGfx( const char *fulltarget, char *filename ){
	Gfx::GenerateAllGfx( fulltarget, filename );

		/* Generate video */
	if(genvideo)
		generateVideo(fulltarget, filename, "qual", "quality");
}
