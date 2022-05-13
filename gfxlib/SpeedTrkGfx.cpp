/* AltitudeGraphic
 * Generate altitude graphics
 */
#include "SpeedTrkGfx.h"

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <cmath>

SpeedTrkGfx::SpeedTrkGfx(GPVideo &v, char atype) : Gfx( 600,300, v ), type(atype) {
	this->calcScales();
}

void SpeedTrkGfx::calcScales( void ){
	this->range = (((int)((type == '3') ? this->video.getMax().spd3d : this->video.getMax().spd2d))/10 + 1)*10;
	this->scale_h = (double)this->SY/(double)this->range;
	this->scale_w = (double)this->SX/(double)this->video.getSampleCount();
}

void SpeedTrkGfx::drawGPMF(cairo_t *cr, int offset, GPVideo::GPMFdata *current){
	if(!current){	/* Drawing shadow */
		cairo_set_source_rgba(cr, 0,0,0, 0.55);
		cairo_set_line_width(cr, 5);
	} else {	/* Drawing curve */
		cairo_set_line_width(cr, 3);
		cairo_set_source_rgb(cr, 0.11, 0.65, 0.88);	/* Set white color */
	}

	GPVideo::GPMFdata *p;
	uint32_t i;
	for(i = 0, p = this->video.getFirst(); i < video.getSampleCount(); i++, p=p->next){
		int x = i*this->scale_w + offset;
		int y = this->SY - ((type == '3') ? p->spd3d : p->spd2d)*this->scale_h + offset;

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

void SpeedTrkGfx::generateBackground( ){
	cairo_t *cr = cairo_create(this->background);

	cairo_set_source_rgba(cr, 1,1,1, 0.80);	/* Set white color */
	cairo_set_line_width(cr, 1);

	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, this->SY);

	cairo_set_font_size(cr, 27);
	for(int i = 0; i <= range; i += 10){
		char t[11];
		sprintf(t, "%d", i);

		int y = this->SY - i*this->scale_h;
	
		cairo_move_to(cr, 0, y);
		cairo_line_to(cr, this->SX, y);

		if(y > 30){
			cairo_move_to(cr, 3, y - 3);
			cairo_show_text (cr, t);
		}
	}
	cairo_stroke(cr);

	drawGPMF(cr, 3, NULL);	// Draw Shadow

	cairo_pattern_t *pat = cairo_pattern_create_linear(0,this->SY - ((type == '3') ? this->video.getMax().spd3d : this->video.getMax().spd2d)*this->scale_h, 0, this->SY);
	cairo_pattern_add_color_stop_rgba(pat, 0, 0,0,0, 0.25);
	cairo_pattern_add_color_stop_rgba(pat, 1, 0,0,0, 0.05);
	cairo_set_source(cr, pat);

	cairo_line_to(cr, this->SX, this->SY);
	cairo_line_to(cr, 0, this->SY);
	cairo_line_to(cr, 0, this->SY - ((type == '3') ? this->video.getFirst()->spd3d : this->video.getFirst()->spd2d)*this->scale_h);

	cairo_fill(cr);

		/* Cleaning */
	cairo_pattern_destroy(pat);
	cairo_destroy(cr);

}

void SpeedTrkGfx::generateOneGfx(const char *fulltarget, char *filename, int index, GPVideo::GPMFdata *current, GPX *){

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
	drawGPMF(cr, 0, current);


		/* Display the spot */
	cairo_set_line_width(cr, 5);
	cairo_arc(cr, index*this->scale_w, this->SY - ((type == '3') ? current->spd3d : current->spd2d)*this->scale_h , 8, 0, 2 * M_PI);
	cairo_stroke_preserve(cr);
	cairo_set_source_rgb(cr, 0.8, 0.2, 0.2);
	cairo_fill(cr);

		/* Writing the image */
	sprintf(filename, "stk%07d.png", index);
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

void SpeedTrkGfx::GenerateAllGfx( const char *fulltarget, char *filename, GPX *hiking ){
	Gfx::GenerateAllGfx( fulltarget, filename, hiking );

		/* Generate video */
	if(genvideo)
		generateVideo(fulltarget, filename, "stk", "strack");
}
