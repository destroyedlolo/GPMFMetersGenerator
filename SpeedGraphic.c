/* SpeedGraphic
 * Generate speed graphics
 *
 */

#include "SpeedGraphic.h"

#include <stdio.h>
#include <stdlib.h>
#include <cairo.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#define GFX_W	300
#define GFX_H	300

bool s3d = false;

void GenerateAllSpeedGfx( const char *fulltarget, char *filename ){
#if 0
	int i;
	struct GPMFdata *p;
	for(i = 0, p = first; i < samples_count; i++, p=p->next)
		GenerateSpeedGfx(fulltarget, filename, i, p);
#endif
	GenerateSpeedGfx(fulltarget, filename, 0, first);
}

void GenerateSpeedGfx( const char *fulltarget, char *filename, int index, struct GPMFdata *current){
int offx = 0, offy = 0;

	/* 
	 * compute scales 
	 */
	int min_h = (((int)(s3d ? min.spd3d : min.spd2d))/10)*10;
	int max_h = (((int)(s3d ? max.spd3d : max.spd2d))/10 + 1)*10;
	int range_h = max_h - min_h;
	double scale_h = (double)GFX_H/(double)range_h;
	double scale_w = (double)(GFX_W - offx)/(double)samples_count;
	int delta_h = ((range_h/5)/10)*10;
	if(!delta_h)
		delta_h = range_h/5;

	if(!delta_h){
		if(verbose || debug)
			puts("*W* Same speed, graphics not generated");
		return;
	}

#if 1	/* remove noise */
	if(debug){
		printf("*D* Normalized min: %.3f -> %d, max: %.3f -> %d\n",
			(s3d ? min.spd3d : min.spd2d), min_h,
			(s3d ? max.spd3d : max.spd2d), max_h
		);
		printf("*D* range : %d, horiz scale : %f, vert scale : %f, delta line : %d (%d)\n",
			range_h, scale_w, scale_h, delta_h, range_h/delta_h
		);
	}
#endif

}
