/* SpeedGraphic
 * Generate speed-o-meter graphics
 *
 */

#include "SpeedGfx.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <cerrno>

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

void SpeedGfx::generateBackGround( ){
	Gfx::generateBackGround();
	
	int speed = (int)(log10(range)) + 1;

	cairo_t *cr = cairo_create(this->background);

	cairo_pattern_t *pat = cairo_pattern_create_radial(this->SX/3,this->SX/3, this->SX/2, 0,0, this->SX/3);
	cairo_pattern_add_color_stop_rgba(pat, 0, 0,0,0, 0.15);
	cairo_pattern_add_color_stop_rgba(pat, 1, 0.6,0.6,0.6, 0.15);
	cairo_set_source(cr, pat);

	cairo_arc(cr, this->SX/2, this->SX/2 , this->SX/2, 0, 2 * M_PI);
	cairo_fill(cr);
	cairo_stroke(cr);

	cairo_set_source_rgba(cr, 1,1,1, 0.75);
	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 30);
	cairo_move_to(cr, 195, 225);
	cairo_show_text(cr, "km/h");
	cairo_stroke(cr);

	cairo_set_font_size(cr, 27);

	double i;
	bool mid;
	cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
	for(i = 0, mid = false, speed = 0; i <= 3 * M_PI/2; i += 3 * M_PI/2/(range/5), mid = mid ? false:true, speed += 5){
		double x = cos( transforme(i) ) * this->SX/2;
		double y = sin( transforme(i) ) * this->SX/2;

		if(!mid){
			char t[11];
			cairo_text_extents_t extents;
			sprintf(t, "%d", speed);
			cairo_text_extents(cr, t, &extents);
			cairo_set_source_rgba(cr, 0.90,0.90,0.90, 0.85);
			cairo_move_to(cr,
				0.6 * x + this->SX/2 - (extents.width/2 + extents.x_bearing), 
				0.6 * y + this->SX/2 - (extents.height/2 + extents.y_bearing)
			);
			cairo_show_text (cr, t);
	
			cairo_set_source_rgba(cr, 0,0,0, 0.75);
			cairo_set_line_width(cr, 8);
			cairo_move_to(cr, 0.8*x + this->SX/2 + 4, 0.8 * y + this->SX/2 + 4);
			cairo_line_to(cr, x + this->SX/2 + 4, y + this->SX/2 + 4);
			cairo_stroke(cr);

			cairo_set_source_rgba(cr, 1,1,1, 1.00);
			cairo_move_to(cr, 0.8*x + this->SX/2, 0.8 * y + this->SX/2);
		} else {
			cairo_set_line_width(cr, 3);
			cairo_move_to(cr, 0.9*x + this->SX/2, 0.9 * y + this->SX/2);
		}
		cairo_line_to(cr, x + this->SX/2, y + this->SX/2);
		cairo_stroke(cr);
	}
	cairo_destroy(cr);
}

void SpeedGfx::generateOneGfx(const char *fulltarget, char *filename, int index, struct GPMFdata *current){
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


		/*
		 * Generate image
		 */

	cairo_set_source_rgb(cr, 1,1,1);	/* Set white color */
	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, LABEL_SZ);
	char t[8];
	sprintf(t, "%4.1f", (type == '3'? current->spd3d : current->spd2d));

	if(this->type == 'b')
		cairo_set_source_rgb(cr, 0.8, 0.2, 0.2);
	else {
		cairo_set_source_rgb(cr, 0,0,0);	/* Background */
		cairo_move_to(cr, offlabel + 4, 170 + 4);
		cairo_show_text(cr, t);

		cairo_set_source_rgb(cr, 1,1,1);	/* Foreground */
	}
	cairo_move_to(cr, offlabel, 170 - ((this->type == 'b') ? 25:0));
	cairo_show_text(cr, t);
	cairo_stroke(cr);

	if(this->type == 'b'){
		cairo_set_source_rgb(cr, 0.2, 0.8, 0.2);
		sprintf(t, "%4.1f", current->spd2d);
		cairo_move_to(cr, offlabel, 200);
		cairo_show_text(cr, t);
		cairo_stroke(cr);
	}

	double val;
	if(this->type == 'b'){
		val = this->transforme(current->spd2d * scale);

		cairo_set_line_width(cr, 13);
		cairo_set_source_rgba(cr, 0.3, 0.6, 0.4, 0.75);
		cairo_arc(cr, this->SX/2, this->SY/2 , this->SX/2 - 35, transforme(0), val );
		cairo_stroke(cr);

		cairo_set_source_rgb(cr, 1,1,1);
		cairo_set_line_width(cr, 10);
		cairo_arc(cr, cos( val ) * (this->SX/2 - 35) + this->SX/2, sin( val ) * (this->SX/2 - 35) + this->SX/2, 10, 0, 2 * M_PI);
		cairo_stroke_preserve(cr);
		cairo_set_source_rgb(cr, 0.2, 0.8, 0.2);
		cairo_fill(cr);
		cairo_stroke(cr);
	}

	val = this->transforme(((this->type == '3') ? current->spd3d : current->spd2d) * scale);

	cairo_set_line_width(cr, 13);
	cairo_set_source_rgba(cr, 0.3, 0.4, 0.6, 0.75);
	cairo_arc(cr, this->SX/2, this->SY/2 , this->SX/2 - 15, this->transforme(0), val );
	cairo_stroke(cr);

	cairo_set_source_rgb(cr, 1,1,1);
	cairo_set_line_width(cr, 10);
	cairo_arc(cr, cos( val ) * (this->SX/2 - 15) + this->SX/2, sin( val ) * (this->SY/2 - 15) + this->SY/2, 10, 0, 2 * M_PI);
	cairo_stroke_preserve(cr);
	cairo_set_source_rgb(cr, 0.8, 0.2, 0.2);
	cairo_fill(cr);
	cairo_stroke(cr);

	sprintf(filename, "spd%07d.png", index);
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

void SpeedGfx::GenerateAllGfx( const char *fulltarget, char *filename ){
	uint32_t i;
	struct GPMFdata *p;

	this->generateBackGround();

	for(i = 0, p = this->video.getFirst(); i < this->video.getSampleCount(); i++, p=p->next)
		generateOneGfx(fulltarget, filename, i, p);

	if(verbose)
		puts("");

		/* Generate video */
	if(genvideo)
		generateVideo(fulltarget, filename, "spd", "speed");
}

