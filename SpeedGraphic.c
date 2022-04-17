/* SpeedGraphic
 * Generate speed-o-meter graphics
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
#define LABEL_SZ 55	/* Label font size */


bool s3d = false;
bool sboth = false;

static int range;
static int offlabel;
static double scale;

static cairo_surface_t *background;

static double transforme( double angle ){
	return -(1.5 * M_PI - angle);
}

static void generateBackGround(){
	background = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, GFX_SZ, GFX_SZ);
    if(cairo_surface_status(background) != CAIRO_STATUS_SUCCESS){
        puts("*F* Can't create Cairo's surface");
        exit(EXIT_FAILURE);
    }

    cairo_t *cr = cairo_create(background);

		/* 
		 * compute scales 
		 */
	if(sboth){
		range = ((int)(max.spd3d/10) + 1)*10;
		int range2 = ((int)(max.spd2d/10) + 1)*10;
		if(range2 > range)
			range = range2;
	} else
		range = (((int)(s3d ? max.spd3d : max.spd2d))/10 + 1)*10;
	scale = 3.0/2.0* M_PI/(double)range;

		/* Calculate speed label offset */
	cairo_text_extents_t extents;
	int speed = (int)(log10(range)) + 1;
	char t[11], *p = t;
	for(;speed; speed--)
		*(p++) = '8';
	strcpy(p, ".8");
	
	cairo_set_font_size(cr, LABEL_SZ);
	cairo_text_extents(cr, t, &extents);
	
	offlabel = (GFX_SZ - extents.x_advance)/2;
	
		/* Background */
	cairo_arc(cr, GFX_SZ/2, GFX_SZ/2 , GFX_SZ/2, 0, 2 * M_PI);
	cairo_set_source_rgba(cr, 0,0,0, 0.25);
//cairo_set_source_rgba(cr, 1,0.25,0.25, 0.25);
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
	for(i = 0, mid = false, speed = 0; i <= 3 * M_PI/2; i += 3 * M_PI/2/(range/5), mid = mid ? false:true, speed += 10){
		double x = cos( transforme(i) ) * GFX_SZ/2;
		double y = sin( transforme(i) ) * GFX_SZ/2;

		if(!mid){
			cairo_text_extents_t extents;
			sprintf(t, "%d", speed);
			cairo_text_extents(cr, t, &extents);
			cairo_set_source_rgba(cr, 0.90,0.90,0.90, 0.75);
			cairo_move_to(cr,
				0.6 * x + GFX_SZ/2 - (extents.width/2 + extents.x_bearing), 
				0.6 * y + GFX_SZ/2 - (extents.height/2 + extents.y_bearing)
			);
			cairo_show_text (cr, t);
	
			cairo_set_source_rgba(cr, 0,0,0, 0.75);
			cairo_set_line_width(cr, 8);
			cairo_move_to(cr, 0.8*x + GFX_SZ/2 + 4, 0.8 * y + GFX_SZ/2 + 4);
			cairo_line_to(cr, x + GFX_SZ/2 + 4, y + GFX_SZ/2 + 4);
			cairo_stroke(cr);

			cairo_set_source_rgba(cr, 1,1,1, 1.00);
			cairo_move_to(cr, 0.8*x + GFX_SZ/2, 0.8 * y + GFX_SZ/2);
		} else {
			cairo_set_line_width(cr, 3);
			cairo_move_to(cr, 0.9*x + GFX_SZ/2, 0.9 * y + GFX_SZ/2);
		}
		cairo_line_to(cr, x + GFX_SZ/2, y + GFX_SZ/2);
		cairo_stroke(cr);
	}

		/* Cleaning */
	cairo_destroy(cr);
}

static void GenerateSpeedGfx( const char *fulltarget, char *filename, int index, struct GPMFdata *current){
		/*
		 * Initialise Cairo
		 */
	cairo_status_t err;

	cairo_surface_t *srf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, GFX_SZ, GFX_SZ);
	if(cairo_surface_status(srf) != CAIRO_STATUS_SUCCESS){
		puts("*F* Can't create Cairo's surface");
		exit(EXIT_FAILURE);
	}

	cairo_t *cr = cairo_create(srf);

	cairo_set_source_surface(cr, background, 0, 0);
	cairo_rectangle(cr, 0,0, GFX_SZ, GFX_SZ);
	cairo_fill(cr);
	cairo_stroke(cr);


		/*
		 * Generate image
		 */

	cairo_set_source_rgb(cr, 1,1,1);	/* Set white color */
	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, LABEL_SZ);
	char t[8];
	sprintf(t, "%4.1f", (s3d ? current->spd3d : current->spd2d));

	if(sboth)
		cairo_set_source_rgb(cr, 0.8, 0.2, 0.2);
	else {
		cairo_set_source_rgb(cr, 0,0,0);	/* Background */
		cairo_move_to(cr, offlabel + 4, 170 + 4);
		cairo_show_text(cr, t);

		cairo_set_source_rgb(cr, 1,1,1);	/* Foreground */
	}
	cairo_move_to(cr, offlabel, 170 - (sboth ? 25:0));
	cairo_show_text(cr, t);
	cairo_stroke(cr);

	if(sboth){
		cairo_set_source_rgb(cr, 0.2, 0.8, 0.2);
		sprintf(t, "%4.1f", current->spd2d);
		cairo_move_to(cr, offlabel, 200);
		cairo_show_text(cr, t);
		cairo_stroke(cr);
	}

	double val;
	if(sboth){
		val = transforme(current->spd2d * scale);

		cairo_set_line_width(cr, 13);
		cairo_set_source_rgba(cr, 0.3, 0.6, 0.4, 0.75);
		cairo_arc(cr, GFX_SZ/2, GFX_SZ/2 , GFX_SZ/2 - 35, transforme(0), val );
		cairo_stroke(cr);

		cairo_set_source_rgb(cr, 1,1,1);
		cairo_set_line_width(cr, 10);
		cairo_arc(cr, cos( val ) * (GFX_SZ/2 - 35) + GFX_SZ/2, sin( val ) * (GFX_SZ/2 - 35) + GFX_SZ/2, 10, 0, 2 * M_PI);
		cairo_stroke_preserve(cr);
		cairo_set_source_rgb(cr, 0.2, 0.8, 0.2);
		cairo_fill(cr);
		cairo_stroke(cr);
	}

	val = transforme((s3d ? current->spd3d : current->spd2d) * scale);

	cairo_set_line_width(cr, 13);
	cairo_set_source_rgba(cr, 0.3, 0.4, 0.6, 0.75);
	cairo_arc(cr, GFX_SZ/2, GFX_SZ/2 , GFX_SZ/2 - 15, transforme(0), val );
	cairo_stroke(cr);

	cairo_set_source_rgb(cr, 1,1,1);
	cairo_set_line_width(cr, 10);
	cairo_arc(cr, cos( val ) * (GFX_SZ/2 - 15) + GFX_SZ/2, sin( val ) * (GFX_SZ/2 - 15) + GFX_SZ/2, 10, 0, 2 * M_PI);
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

void GenerateAllSpeedGfx( const char *fulltarget, char *filename ){
	int i;
	struct GPMFdata *p;

	generateBackGround();

	for(i = 0, p = first; i < samples_count; i++, p=p->next)
		GenerateSpeedGfx(fulltarget, filename, i, p);

		/* Cleaning */
	cairo_surface_destroy(background);

		/* Generate video */
	if(video)
		generateVideo(fulltarget, filename, "spd", "speed");
}
