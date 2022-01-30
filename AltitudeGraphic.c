/* AltitudeGraphic
 * Generate altitude graphics
 *
 */

#include "AltitudeGraphic.h"

#include <stdio.h>

#define GFX_W	600
#define GFX_H	300

void GenerateAltitudeGfx( const char *fulltarget, char *filename, int index ){

	/* compute scales */

	int min_h = (((int)min.altitude)/50)*50;
	int max_h = (((int)max.altitude)/50 + 1)*50;
	int range_h = max_h - min_h;
	double scale_h = (double)range_h/(double)GFX_H;
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
}
