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

#define R (6371000)	/* terrestrial radius */

	/* From https://forums.futura-sciences.com/mathematiques-superieur/39838-conversion-lat-long-x-y.html */
void posXY( double lat, double lgt, int *x, int *y){
	/* Degree -> Radian */
	lat *= M_PI/180.0;
	lgt *= M_PI/180.0;

	*x = (int)(2 * R * sin(lgt)*cos(lat) / (1 + cos(lgt)*cos(lat)));
	*y = (int)(2 * R * sin(lat) / (1 + cos(lgt)*cos(lat)));
}


void GenerateAllPathGfx( const char *fulltarget, char *filename ){
#if 0
	int i;
	struct GPMFdata *p;
	for(i = 0, p = first; i < samples_count; i++, p=p->next)
		GeneratePathGfx(fulltarget, filename, i, p);
#endif
		GeneratePathGfx(fulltarget, filename, 0, first);
}

void GeneratePathGfx( const char *fulltarget, char *filename, int index, struct GPMFdata *current){
		/* 
		 * compute limits, scales
		 */
	int min_x, min_y;
	int max_x, max_y;

	posXY( min.latitude, min.longitude, &min_x, &min_y);
	posXY( max.latitude, max.longitude, &max_x, &max_y);

	int range_x = max_x - min_x;
	int range_y = max_y - min_y;

	printf("min: (%d,%d), max: (%d,%d) -> (%d, %d)\n",
		min_x, min_y, max_x, max_y, range_x, range_y
	);
}
