/* SpeedTracker
 * Generate speed tracker graphics
 *
 * See include file for description
 */

#include "Shared.h"
#include "SpeedTracker.h"

#include <stdio.h>
#include <stdlib.h>
#include <cairo.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#define GFX_W	600
#define GFX_H	200

static cairo_surface_t *background;
static int range;
static double scale_h, scale_w;

static void generateBackGround(){
	background = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, GFX_W, GFX_H);
    if(cairo_surface_status(background) != CAIRO_STATUS_SUCCESS){
        puts("*F* Can't create Cairo's surface");
        exit(EXIT_FAILURE);
    }

    cairo_t *cr = cairo_create(background);

		/* 
		 * compute scales 
		 */
	range = (((int)(s3d ? max.spd3d : max.spd2d))/10 + 1)*10;
	scale_h = (double)GFX_H/(double)range;
	scale_w = (double)GFX_W/(double)samples_count;


		/*
		 * Background
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

	cairo_set_font_size(cr, 27);
	for(i = 0; i <= range; i += 10){
		char t[11];
		sprintf(t, "%d", i);

		int y = GFX_H - i*scale_h;
	
		cairo_move_to(cr, 0, y);
		cairo_line_to(cr, GFX_W, y);

		if(y > 30){
			cairo_move_to(cr, 3, y - 3);
			cairo_show_text (cr, t);
		}
	}
	cairo_stroke(cr);

	struct GPMFdata *p;

	cairo_set_line_width(cr, 3);
	cairo_set_source_rgba(cr, 0,0,0, 0.75);
	for(i = 0, p = first; i < samples_count; i++, p=p->next){
		int x = i*scale_w;
		int y = GFX_H - (s3d ? p->spd3d : p->spd2d)*scale_h;

		if(!i)	/* First plot */
			cairo_move_to(cr, x+2, y+2);
		else
			cairo_line_to(cr, x+2, y+2);
	}
	cairo_stroke(cr);

	cairo_set_line_width(cr, 2);
	cairo_set_source_rgb(cr, 1,1,1);
	for(i = 0, p = first; i < samples_count; i++, p=p->next){
		int x = i*scale_w;
		int y = GFX_H - (s3d ? p->spd3d : p->spd2d)*scale_h;

		if(!i)	/* First plot */
			cairo_move_to(cr, x, y);
		else
			cairo_line_to(cr, x, y);
	}
	cairo_stroke(cr);
	

		/* Cleaning */
	cairo_destroy(cr);
}


static void updateBackGround(struct GPMFdata *current){
	if(current == first)
		return;

    cairo_t *cr = cairo_create(background);
	struct GPMFdata *p;
	int i;

	cairo_set_line_width(cr, 3);
	cairo_set_source_rgb(cr, 0.3, 0.4, 0.6);
	
	for(i = 0, p = first; i < samples_count; i++, p=p->next){
		int x = i*scale_w;
		int y = GFX_H - (s3d ? p->spd3d : p->spd2d)*scale_h;

		if(p == current){
			cairo_line_to(cr, x, y);
			break;
		} else
			cairo_move_to(cr, x, y);
	}
	cairo_stroke(cr);
	
		/* Cleaning */
	cairo_destroy(cr);
}

static void GenerateSpeedTrkGfx( const char *fulltarget, char *filename, int index, struct GPMFdata *current){

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
		 * Display the spot
		 */

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

void GenerateAllSpeedTrkGfx( const char *fulltarget, char *filename ){
	int i;
	struct GPMFdata *p;

	generateBackGround();

	for(i = 0, p = first; i < samples_count; i++, p=p->next){
		updateBackGround(p);
		GenerateSpeedTrkGfx(fulltarget, filename, i, p);
	}

		/* Generate video */
	if(video)
		generateVideo(fulltarget, filename, "stk", "strack");
}
