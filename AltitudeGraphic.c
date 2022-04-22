/* AltitudeGraphic
 * Generate altitude graphics
 *
 */

#include "Shared.h"
#include "AltitudeGraphic.h"

#include <stdio.h>
#include <stdlib.h>
#include <cairo.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#define GFX_W	600
#define GFX_H	300

static int posLabel, offx, offy;

static int min_h, max_h, range_h;
static double scale_h, scale_w;
static int delta_h;

static cairo_surface_t *background;

static void drawGPMF(cairo_t *cr, int offset){
	struct GPMFdata *p;
	int i;

	for(i = 0, p = first; i < samples_count; i++, p=p->next){
		int x = offx + i*scale_w + offset;
		int y = GFX_H - (p->altitude - min_h)*scale_h + offset;

		if(!i)	/* First plot */
			cairo_move_to(cr, x, y);
		else
			cairo_line_to(cr, x, y);
	}
	cairo_stroke(cr);
}

static void generateBackGround(){
	cairo_text_extents_t extents;

	background = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, GFX_W, GFX_H);
	if(cairo_surface_status(background) != CAIRO_STATUS_SUCCESS){
		puts("*F* Can't create Cairo's surface");
		exit(EXIT_FAILURE);
	}

	cairo_t *cr = cairo_create(background);

	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 35);
	cairo_text_extents(cr, "8888", &extents);
	offy = extents.height + 5;
	posLabel = GFX_W - extents.x_advance - 55;

	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 15);
	cairo_text_extents(cr, "8888", &extents);
	offx = extents.x_advance + 10;

		/* 
		 * compute scales 
		 */

	min_h = (((int)min.altitude)/50)*50;
	max_h = (((int)max.altitude)/50 + 1)*50;
	range_h = max_h - min_h;
	scale_h = (double)(GFX_H - offy)/(double)range_h;
	scale_w = (double)(GFX_W - offx)/(double)samples_count;
	delta_h = ((range_h/5)/50)*50;
	if(!delta_h)
		delta_h = range_h/5;

		/*
		 * Generate image
		 */

	cairo_set_source_rgba(cr, 0,0,0, 0.25);	/* Dark background */
	cairo_rectangle(cr, 0,0, GFX_W, GFX_H);
	cairo_fill(cr);

		/* Draw iso level lines */
	cairo_set_source_rgba(cr, 1,1,1, 0.75);	/* Set white color */
	cairo_set_line_width(cr, 1);

	int i;
	char t[8];
	for(i = min_h; i <= max_h; i += delta_h){
		int y = GFX_H - (i-min_h)*scale_h;
		sprintf(t, "%5d", i);

		cairo_move_to(cr, 3, y);
		cairo_show_text(cr, t);
		cairo_move_to(cr, offx, y);
		cairo_line_to(cr, GFX_W, y);
	}
	cairo_stroke(cr);

		/* Draw altitude line */

			/* Draw Shadow */
	cairo_set_line_width(cr, 3);
	cairo_set_source_rgba(cr, 0,0,0, 0.55);
	drawGPMF(cr, 3);

			/* Draw Altitude */
	cairo_set_line_width(cr, 2);
	cairo_set_source_rgb(cr, 1,1,1);	/* Set white color */
	drawGPMF(cr, 0);

		/* Cleaning */
	cairo_destroy(cr);
}

static void GenerateAltitudeGfx( const char *fulltarget, char *filename, int index, struct GPMFdata *current){

		/*
		 * Initialise Cairo
		 */
	cairo_status_t err;

	cairo_surface_t *srf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, GFX_W, GFX_H);
	if(cairo_surface_status(srf) != CAIRO_STATUS_SUCCESS){
		puts("*F* Can't create Cairo's surface");
		exit(EXIT_FAILURE);
	}

	cairo_t *cr = cairo_create(srf);

	cairo_set_source_surface(cr, background, 0, 0);
	cairo_rectangle(cr, 0,0, GFX_W, GFX_H);
	cairo_fill(cr);
	cairo_stroke(cr);

		/*
		 * Generate image
		 */

		/* Display the label */
	char t[8];
	cairo_set_source_rgb(cr, 1,1,1);	/* Set white color */
	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 35);
	cairo_move_to(cr, posLabel, offy);
	sprintf(t, "%5d m", (int)current->altitude);
	cairo_show_text(cr, t);
	cairo_stroke(cr);

		/* Display the spot */
	cairo_set_line_width(cr, 5);
	cairo_arc(cr, offx + index*scale_w, GFX_H - (current->altitude - min_h)*scale_h , 8, 0, 2 * M_PI);
	cairo_stroke_preserve(cr);
	cairo_set_source_rgb(cr, 0.8, 0.2, 0.2);
	cairo_fill(cr);

	sprintf(filename, "alt%07d.png", index);
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

void GenerateAllAltitudeGfx( const char *fulltarget, char *filename ){
	int i;
	struct GPMFdata *p;

	generateBackGround();

	for(i = 0, p = first; i < samples_count; i++, p=p->next)
		GenerateAltitudeGfx(fulltarget, filename, i, p);

		/* Cleaning */
	cairo_surface_destroy(background);

		/* Generate video */
	if(video)
		generateVideo(fulltarget, filename, "alt", "altitude");
}


