/* SpeedGraphic
 * Generate speed graphics
 *
 */

#include "SpeedGraphic.h"

#include <stdio.h>
#include <stdlib.h>
#include <cairo.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#define GFX_SZ	300

bool s3d = false;

void GenerateAllSpeedGfx( const char *fulltarget, char *filename ){
#if 0
	int i;
	struct GPMFdata *p;
	for(i = 0, p = first; i < samples_count; i++, p=p->next)
		GenerateSpeedGfx(fulltarget, filename, i, p);
#endif
	GenerateSpeedGfx("/tmp/tst.png", filename, 0, first);
}

static double transforme( double angle ){
	return -(1.5 * M_PI - angle);
}

void GenerateSpeedGfx( const char *fulltarget, char *filename, int index, struct GPMFdata *current){
		/*
		 * Initialise Cairo
		 */
	cairo_status_t err;
	cairo_text_extents_t extents;

	cairo_surface_t *srf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, GFX_SZ, GFX_SZ);
	if(cairo_surface_status(srf) != CAIRO_STATUS_SUCCESS){
		puts("*F* Can't create Cairo's surface");
		exit(EXIT_FAILURE);
	}

	cairo_t *cr = cairo_create(srf);

	/* 
	 * compute scales 
	 */
	int range = (((int)(s3d ? max.spd3d : max.spd2d))/10 + 1)*10;
	double scale = (double)(3/2)* M_PI/(double)range;

#if 1	/* remove noise */
	if(debug){
		printf("*D* Normalized min: %.3f -> 0, max: %.3f -> %d\n",
			(s3d ? min.spd3d : min.spd2d),
			(s3d ? max.spd3d : max.spd2d), range
		);
	}
#endif

		/* Background */
	cairo_arc(cr, GFX_SZ/2, GFX_SZ/2 , GFX_SZ/2, 0, 2 * M_PI);
	cairo_set_source_rgba(cr, 0,0,0, 0.25);
	cairo_fill(cr);
	cairo_stroke(cr);

	cairo_set_source_rgba(cr, 1,1,1, 0.75);
	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 30);
	cairo_move_to(cr, 195, 225);
	cairo_show_text(cr, "km/h");
	cairo_stroke(cr);

	double i;
	cairo_set_line_width(cr, 8);
	cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
	for(i = 0; i <= 3 * M_PI/2; i += 3 * M_PI/2/(range/10)){
		double x = cos( transforme(i) ) * GFX_SZ/2;
		double y = sin( transforme(i) ) * GFX_SZ/2;

		cairo_move_to(cr, x + GFX_SZ/2, y + GFX_SZ/2);
		cairo_line_to(cr, 0.8*x + GFX_SZ/2, 0.8 * y + GFX_SZ/2);
	}
	cairo_stroke(cr);

	cairo_set_source_rgb(cr, 1,1,1);	/* Set white color */
	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 65);
	char t[8];
	sprintf(t, "%6.1f", (s3d ? current->spd3d : current->spd2d));
	cairo_text_extents(cr, t, &extents);
	cairo_move_to(cr, 245 - extents.x_advance, 170);
	cairo_show_text(cr, t);
	cairo_stroke(cr);


	if((err = cairo_surface_write_to_png(srf, fulltarget)) != CAIRO_STATUS_SUCCESS){
		printf("*F* Writing surface : %s / %s\n", cairo_status_to_string(err), strerror(errno));
		exit(EXIT_FAILURE);
	}

		/* Cleaning */
	cairo_destroy(cr);
	cairo_surface_destroy(srf);
}