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
#include <math.h>

#define GFX_W	600
#define GFX_H	300

void GenerateAllAltitudeGfx( const char *fulltarget, char *filename ){
	int i;
	struct GPMFdata *p;
	for(i = 0, p = first; i < samples_count; i++, p=p->next)
		GenerateAltitudeGfx(fulltarget, filename, i, p);
}

void GenerateAltitudeGfx( const char *fulltarget, char *filename, int index, struct GPMFdata *current){

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

	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 35);
	cairo_text_extents(cr, "8888", &extents);
	int offy = extents.height + 5;
	int posLabel = GFX_W - extents.x_advance - 55;

	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 15);
	cairo_text_extents(cr, "8888", &extents);
	int offx = extents.x_advance + 5;

	/* 
	 * compute scales 
	 */

	int min_h = (((int)min.altitude)/50)*50;
	int max_h = (((int)max.altitude)/50 + 1)*50;
	int range_h = max_h - min_h;
	double scale_h = (double)(GFX_H - offy)/(double)range_h;
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
	 */

	int i;
	cairo_set_source_rgba(cr, 0,0,0, 0.25);	/* Dark background */
	cairo_rectangle(cr, 0,0, GFX_W, GFX_H);
	cairo_fill(cr);

		/* Draw iso level lines */
	char t[8];
	cairo_set_source_rgba(cr, 1,1,1, 0.75);	/* Set white color */
	cairo_set_line_width(cr, 1);

	for(i = min_h; i <= max_h; i += delta_h){
		int y = GFX_H - (i-min_h)*scale_h;
		sprintf(t, "%5d", i);

		cairo_move_to(cr, 0, y);
		cairo_show_text(cr, t);
		cairo_move_to(cr, offx, y);
		cairo_line_to(cr, GFX_W, y);
	}
	cairo_stroke(cr);

		/* Display the label */
	cairo_set_source_rgb(cr, 1,1,1);	/* Set white color */
	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 35);
	cairo_move_to(cr, posLabel, offy);
	sprintf(t, "%5d m", (int)current->altitude);
	cairo_show_text(cr, t);
	cairo_stroke(cr);

	struct GPMFdata *p;
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

		/* Display the spot */
	cairo_arc(cr, offx + index*scale_w, GFX_H - (current->altitude - min_h)*scale_h , 5, 0, 2 * M_PI);
	cairo_stroke_preserve(cr);
	cairo_set_source_rgb(cr, 0.3, 0.4, 0.6);
	cairo_fill(cr);

	sprintf(filename, "alt%07d.png", index);
	if(debug)
		printf("*D* Writing '%s'\n", fulltarget);

	if((err = cairo_surface_write_to_png(srf, fulltarget)) != CAIRO_STATUS_SUCCESS){
		printf("*F* Writing surface : %s / %s\n", cairo_status_to_string(err), strerror(errno));
		exit(EXIT_FAILURE);
	}

		/* Cleaning */
	cairo_destroy(cr);
	cairo_surface_destroy(srf);
}
