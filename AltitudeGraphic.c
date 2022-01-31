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
		 * Initialise Cairo
		 */
	cairo_status_t err;
	cairo_text_extents_t extents;

	cairo_surface_t *srf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, GFX_W, GFX_H);
	if(cairo_surface_status(srf) != CAIRO_STATUS_SUCCESS){
		puts("*F* Can't create Cairo's surface");
		exit(EXIT_FAILURE);
	}

	cairo_t *cr = cairo_create(srf);

	cairo_font_face_t *font = cairo_toy_font_face_create("", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 10);
	cairo_text_extents(cr, "8888", &extents);
	int offx = extents.x_advance + 5;

	/* 
	 * compute scales 
	 */

	int min_h = (((int)min.altitude)/50)*50;
	int max_h = (((int)max.altitude)/50 + 1)*50;
	int range_h = max_h - min_h;
	double scale_h = (double)GFX_H/(double)range_h;
	double scale_w = (double)(GFX_W - offx)/(double)samples_count;
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
		printf("*D* range : %d, horiz scale : %f, vert scale : %f, delta line : %d (%d)\n",
			range_h, scale_w, scale_h, delta_h, range_h/delta_h
		);
	}

	/*
	 * Generate image
	 *
	 *AF* Add margins
	 */

	int i;
	cairo_set_source_rgba(cr, 0,0,0, 0.25);	/* Dark background */
	cairo_rectangle(cr, 0,0, GFX_W, GFX_H);
	cairo_fill(cr);

		/* Draw iso level lines */
	cairo_set_source_rgba(cr, 1,1,1, 0.75);	/* Set white color */
	cairo_set_line_width(cr, 1);

	for(i = min_h; i <= max_h; i += delta_h){
		int y = GFX_H - (i-min_h)*scale_h;
		char t[6];
		sprintf(t, "%5d", i);

		cairo_move_to(cr, 0, y);
		cairo_show_text(cr, t);
		cairo_move_to(cr, offx, y);
		cairo_line_to(cr, GFX_W, y);
	}
	cairo_stroke(cr);

	struct GPMFdata *p;
	cairo_set_source_rgb(cr, 1,1,1);	/* Set white color */
	cairo_set_line_width(cr, 2);
	for(i = 0, p = first; i < samples_count; i++, p=p->next){
		int x = offx + i*scale_w;
		int y = GFX_H - (p->altitude - min_h)*scale_h;

		if(!i)	/* First plot */
			cairo_move_to(cr, x, y);
		else
			cairo_line_to(cr, x, y);
	}
	cairo_stroke(cr);

	if((err = cairo_surface_write_to_png(srf, "/tmp/tst.png")) != CAIRO_STATUS_SUCCESS){
		printf("*F* Writing surface : %s / %s\n", cairo_status_to_string(err), strerror(errno));
		exit(EXIT_FAILURE);
	}

		/* Cleaning */
	cairo_destroy(cr);
	cairo_font_face_destroy(font);
	cairo_surface_destroy(srf);
}
