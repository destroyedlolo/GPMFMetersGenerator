/* PathGraphic
 * Generate simple path graphics
 *
 */

#include "PathGraphic.h"

#include <stdio.h>
#include <stdlib.h>
#include <cairo.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#define GFX 300
#define R (6371000)	/* terrestrial radius */

	/* From https://forums.futura-sciences.com/mathematiques-superieur/39838-conversion-lat-long-x-y.html */
void posXY( double lat, double lgt, int *x, int *y){
	/* Degree -> Radian */
	lat *= M_PI/180.0;
	lgt *= M_PI/180.0;

	double sinlgt = sin(lgt),
		coslgt = cos(lgt),
		sintlat = sin(lat),
		coslat = cos(lat);

	*x = (int)(2 * R * sinlgt*coslat / (1 + coslgt*coslat));
	*y = (int)(2 * R * sintlat / (1 + coslgt*coslat));
}


static int min_x, min_y;
static int max_x, max_y;
static int range_x, range_y;
static int off_x, off_y;
static double scale;

cairo_surface_t *background;

static void generateBackGround(){
		/*
		 * compute limits, scales
		 */
	posXY( min.latitude, min.longitude, &min_x, &min_y);
	posXY( max.latitude, max.longitude, &max_x, &max_y);

	range_x = max_x - min_x;
	range_y = max_y - min_y;

	scale = (double)GFX/(double)((range_x > range_y) ? range_x : range_y);

	off_x = (GFX - range_x * scale)/2;
	off_y = (GFX - range_y * scale)/2;

	printf("min: (%d,%d), max: (%d,%d) -> (%d, %d) (scale: %.2f)\n",
		min_x, min_y, max_x, max_y, range_x, range_y, scale
	);

	background = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, GFX, GFX);
	if(cairo_surface_status(background) != CAIRO_STATUS_SUCCESS){
		puts("*F* Can't create Cairo's surface");
		exit(EXIT_FAILURE);
	}

	cairo_t *cr = cairo_create(background);

	cairo_set_source_rgb(cr, 1,1,1);	/* Set white color */
	cairo_set_line_width(cr, 2);
	struct GPMFdata *p;
	for(p = first; p; p=p->next){
		int x,y;

		posXY(p->latitude, p->longitude, &x, &y);
		x = off_x + (x-min_x) * scale;
		y = off_y + GFX - (y-min_y)*scale;

		if(p == first)
			cairo_move_to(cr, x, y);
		else
			cairo_line_to(cr, x, y);
	}
	cairo_stroke(cr);

cairo_surface_write_to_png(background, "/tmp/tst.png");

		/* Cleaning */
	cairo_destroy(cr);
}

void GenerateAllPathGfx( const char *fulltarget, char *filename ){
	generateBackGround();
#if 0
	int i;
	struct GPMFdata *p;
	for(i = 0, p = first; i < samples_count; i++, p=p->next)
		GeneratePathGfx(fulltarget, filename, i, p);
#endif
		GeneratePathGfx(fulltarget, filename, 0, first);

		/* Cleaning */
	cairo_surface_destroy(background);
}

void GeneratePathGfx( const char *fulltarget, char *filename, int index, struct GPMFdata *current){
}
