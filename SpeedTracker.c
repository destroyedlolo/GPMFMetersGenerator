/* SpeedTracker
 * Generate speed tracker graphics
 *
 * See include file for description
 */

#include "SpeedTracker.h"

#include <stdio.h>
#include <stdlib.h>
#include <cairo.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#define GFX_W	600
#define GFX_H	300

void GenerateAllSpeedTrkGfx( const char *fulltarget, char *filename ){
	int i;
	struct GPMFdata *p;
	for(i = 0, p = first; i < samples_count; i++, p=p->next)
		GenerateSpeedTrkGfx(fulltarget, filename, i, p);
}

void GenerateSpeedTrkGfx( const char *fulltarget, char *filename, int index, struct GPMFdata *current){

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


		/* 
		 * compute scales 
		 */
	int range = (((int)(s3d ? max.spd3d : max.spd2d))/10 + 1)*10;
	double scale_h = (double)GFX_H/(double)range;
	double scale_w = (double)GFX_W/(double)samples_count;


		/*
		 * Generate image
		 */

	int i;
	cairo_set_source_rgba(cr, 0,0,0, 0.25);	/* Dark background */
	cairo_rectangle(cr, 0,0, GFX_W, GFX_H);
	cairo_fill(cr);
	cairo_stroke(cr);

	cairo_set_source_rgba(cr, 1,1,1, 0.75);	/* Set white color */
	cairo_set_line_width(cr, 1);

	cairo_move_to(cr, 0, 0);
	cairo_line_to(cr, 0, GFX_H);

	for(i = 0; i <= range; i += 10){
		int y = GFX_H - i*scale_h;

		cairo_move_to(cr, 0, y);
		cairo_line_to(cr, GFX_W, y);
	}
	cairo_stroke(cr);

	cairo_set_source_rgb(cr, 1,1,1);	/* Set white color */
	struct GPMFdata *p;
	cairo_set_line_width(cr, 2);
	for(i = 0, p = first; i < samples_count; i++, p=p->next){
		int x = i*scale_w;
		int y = GFX_H - (s3d ? p->spd3d : p->spd2d)*scale_h;

		if(!i)	/* First plot */
			cairo_move_to(cr, x, y);
		else
			cairo_line_to(cr, x, y);
	}
	cairo_stroke(cr);

		/* Display the spot */
	cairo_arc(cr, index*scale_w, GFX_H - (s3d ? current->spd3d : current->spd2d)*scale_h , 8, 0, 2 * M_PI);
	cairo_stroke_preserve(cr);
	cairo_set_source_rgb(cr, 0.8, 0.2, 0.2);
	cairo_fill(cr);
	
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
