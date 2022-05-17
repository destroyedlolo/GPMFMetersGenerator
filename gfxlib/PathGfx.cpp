/* PathGraphic
 * Generate path graphics
 */
#include "PathGfx.h"

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <cmath>

PathGfx::PathGfx(GPVideo &v, GPX *h) : Gfx( 300,300, v, h ) {
	this->calcScales();
}

	/* From https://forums.futura-sciences.com/mathematiques-superieur/39838-conversion-lat-long-x-y.html */
void PathGfx::posXY( double lat, double lgt, int &x, int &y){

	/* Degree -> Radian */
	lat = GPSCoordinate::toRadian(lat);
	lgt = GPSCoordinate::toRadian(lgt);

	double sinlgt = sin(lgt),
		coslgt = cos(lgt),
		sintlat = sin(lat),
		coslat = cos(lat);

	x = (int)(2 * R * sinlgt*coslat / (1 + coslgt*coslat));
	y = (int)(2 * R * sintlat / (1 + coslgt*coslat));
}

void PathGfx::calcScales( void ){
	if(this->hiking){
		this->posXY( this->hiking->getMin().getLatitude(), this->hiking->getMin().getLongitude(), this->min_x, this->min_y);
		this->posXY( this->hiking->getMax().getLatitude(), this->hiking->getMax().getLongitude(), this->max_x, this->max_y);
	} else {
		this->posXY( this->video.getMin().getLatitude(), this->video.getMin().getLongitude(), this->min_x, this->min_y);
		this->posXY( this->video.getMax().getLatitude(), this->video.getMax().getLongitude(), this->max_x, this->max_y);
	}

	this->range_x = this->max_x - this->min_x;
	this->range_y = this->max_y - this->min_y;

	this->scale = (double)(this->SX - 20)/(double)((this->range_x > this->range_y) ? this->range_x : this->range_y);

	this->off_x = (this->SX - this->range_x * this->scale)/2;
	this->off_y = (this->SX - this->range_y * this->scale)/2;
}

void PathGfx::drawGPX(cairo_t *cr, int offset){
	for(GPX::GpxData *p = this->hiking->getFirst(); p; p = p->next){
		int x,y;

		posXY(p->getLatitude(), p->getLongitude(), x, y);
		x = this->off_x + (x-this->min_x) * this->scale + offset;
		y = this->SY - this->off_y - (y-this->min_y)*this->scale + offset;

		if(p == this->hiking->getFirst())
			cairo_move_to(cr, x, y);
		else
			cairo_line_to(cr, x, y);
	}
	cairo_stroke(cr);
}

void PathGfx::drawGPMF(cairo_t *cr, int offset, GPVideo::GPMFdata *current){
	GPVideo::GPMFdata *p;

	cairo_save(cr);
	if(current)
		cairo_set_source_rgb(cr, 0.11, 0.65, 0.88);

	for(p = this->video.getFirst(); p; p = p->next){
		int x,y;

		this->posXY(p->getLatitude(), p->getLongitude(), x, y);
		x = this->off_x + (x-this->min_x) * this->scale + offset;
		y = this->SY - this->off_y - (y-this->min_y) * this->scale + offset;

		if(p == this->video.getFirst())
			cairo_move_to(cr, x, y);
		else
			cairo_line_to(cr, x, y);

		if(current == p){
			cairo_stroke(cr);
			cairo_restore(cr);
			cairo_move_to(cr, x, y);
		}
	}
	cairo_stroke(cr);
}

void PathGfx::generateBackground( void ){
	cairo_t *cr = cairo_create(this->background);

	if(this->hiking){
		cairo_set_line_width(cr, 2);
		cairo_set_source_rgba(cr, 0,0,0, 0.55);
		drawGPX(cr, 2);

			/* Draw path */
		cairo_set_source_rgb(cr, 1,1,1);	/* Set white color */
		cairo_set_line_width(cr, 2);
		drawGPX(cr, 0);
	} else {
			/* Draw shadow */
		cairo_set_line_width(cr, 4);
		cairo_set_source_rgba(cr, 0,0,0, 0.55);
		this->drawGPMF(cr, 2, NULL);
	}

		/* Cleaning */
	cairo_destroy(cr);
}

void PathGfx::generateOneGfx( const char *fulltarget, char *filename, int index, GPVideo::GPMFdata *current ){
	cairo_surface_t *srf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, this->SX, this->SY);
	if(cairo_surface_status(srf) != CAIRO_STATUS_SUCCESS){
		puts("*F* Can't create Cairo's surface");
		exit(EXIT_FAILURE);
	}

	cairo_t *cr = cairo_create(srf);
	cairo_set_source_surface(cr, this->background, 0, 0);
	cairo_rectangle(cr, 0, 0, this->SX, this->SY);
	cairo_fill(cr);
	cairo_stroke(cr);

	if(this->hiking)
		cairo_set_source_rgb(cr, 0.3, 0.4, 0.6);
	else
		cairo_set_source_rgb(cr, 1,1,1);
	cairo_set_line_width(cr, 3);
	this->drawGPMF(cr, 0, current);

	cairo_set_source_rgb(cr, 1,1,1);
	int x,y;
	this->posXY(current->getLatitude(), current->getLongitude(), x, y);
	x = this->off_x + (x-this->min_x) * this->scale;
	y = this->SY - this->off_y - (y-this->min_y) * this->scale;

	cairo_set_line_width(cr, 5);
	cairo_arc(cr, x, y, 5, 0, 2 * M_PI);
	cairo_stroke_preserve(cr);
	cairo_set_source_rgb(cr, 0.8, 0.2, 0.2);
	cairo_fill(cr);

	sprintf(filename, "pth%07d.png", index);
	if(verbose)
		printf("*D* Writing '%s'\r", fulltarget);
	
	cairo_status_t err;
	if((err = cairo_surface_write_to_png(srf, fulltarget)) != CAIRO_STATUS_SUCCESS){
		printf("*F* Writing surface : %s / %s\n", cairo_status_to_string(err), strerror(errno));
		exit(EXIT_FAILURE);
	}

		/* Cleaning */
	cairo_destroy(cr);
	cairo_surface_destroy(srf);
}

void PathGfx::GenerateAllGfx( const char *fulltarget, char *filename ){
	Gfx::GenerateAllGfx( fulltarget, filename );

		/* Generate video */
	if(genvideo)
		generateVideo(fulltarget, filename, "pth", "path");
}
