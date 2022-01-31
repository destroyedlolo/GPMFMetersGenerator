/* AltitudeGraphic
 * Generate altitude graphics
 *
 */

#include "AltitudeGraphic.h"

#include <stdio.h>
#include <stdlib.h>
#include <cairo.h>
#include <errno.h>
#include <string.h>

#define GFX_W	600
#define GFX_H	300

void GenerateAltitudeGfx( const char *fulltarget, char *filename, int index ){

	/* 
	 * compute scales 
	 */

	int min_h = (((int)min.altitude)/50)*50;
	int max_h = (((int)max.altitude)/50 + 1)*50;
	int range_h = max_h - min_h;
	double scale_h = (double)GFX_H/(double)range_h;
	int delta_h = ((range_h/5)/50)*50;
	if(!delta_h)
		delta_h = range_h/5;

	if(!delta_h){
		if(verbose || debug)
			puts("*W* Same altitude, graphics not generated");
		return;
	}

	if(debug){
		printf("*D* Normalized min: %.3f -> %d, max: %.3f -> %d\n",
			min.altitude, min_h,
			max.altitude, max_h
		);
		printf("*D* range : %d, vert scale : %f, delta line : %d (%d)\n",
			range_h, scale_h, delta_h, range_h/delta_h
		);
	}

	/*
	 * Generate image
	 *
	 *AF* Add margins
	 */

	int i;
	cairo_status_t err;

		/* fully transparent surface created by default */
	cairo_surface_t *srf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, GFX_W, GFX_H);
	if(cairo_surface_status(srf) != CAIRO_STATUS_SUCCESS){
		puts("*F* Can't create Cairo's surface");
		exit(EXIT_FAILURE);
	}

	cairo_t *cr = cairo_create(srf);

		/* Draw iso level lines */
	cairo_set_source_rgba(cr, 1,1,1, 0.75);	/* Set white color */
	cairo_set_line_width(cr, 1);

	for(i = min_h; i <= max_h; i += delta_h){
		int y = GFX_H - (i-min_h)*scale_h;
		cairo_move_to(cr, 0, y);
		cairo_line_to(cr, GFX_W, y);
		cairo_stroke(cr);
	}

	if((err = cairo_surface_write_to_png(srf, "/tmp/tst.png")) != CAIRO_STATUS_SUCCESS){
		printf("*F* Writing surface : %s / %s\n", cairo_status_to_string(err), strerror(errno));
		exit(EXIT_FAILURE);
	}

		/* Cleaning */
	cairo_destroy(cr);
	cairo_surface_destroy(srf);
}
