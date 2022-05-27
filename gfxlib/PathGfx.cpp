/* PathGraphic
 * Generate path graphics
 *
 * Note about stories :
 * - background's shadow is not changed
 * - traces BEFORE the actual video are in white
 * - traces AFTER the actual video remain gray
 * - traces of other previous videos are in darker blue
 * - traces of the actual video remain blue
 * - traces of videos after the current one is in light blue
 *
 * The algorithm relies on the video to be part of the story
 * (otherwise, it is considered as the last)
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
	if(offset){	// drawing shadow
		cairo_set_line_width(cr, 4);
		cairo_set_source_rgba(cr, 0,0,0, 0.55);
	} else {	// drawing path
		cairo_set_source_rgb(cr, 1,1,1);
		cairo_set_line_width(cr, 2);
	}

	bool first = true;
	GPX::pkind prec = GPX::pkind::AFTERTRACE;

	for(int idx=0; idx < (int)this->hiking->getSampleCount(); idx++){
		int x,y;
		auto &p = this->hiking->getSamples()[idx];

		posXY(p.getLatitude(), p.getLongitude(), x, y);
		x = this->off_x + (x-this->min_x) * this->scale + offset;
		y = this->SY - this->off_y - (y-this->min_y)*this->scale + offset;

		GPX::pkind kind = this->hiking->positionKind(idx);

		if(kind != prec && !offset){
			prec = kind;
			if(!first){
				cairo_stroke(cr);	// Draw previous trace
				first = true;		// start a new one
			}
			switch(kind){
			case GPX::pkind::AFTERTRACE :
				cairo_set_source_rgb(cr, .85,.85,.85);
				break;
			case GPX::pkind::BEFOREVIDEO :
				cairo_set_source_rgb(cr, 0.11, 0.65, 0.88);
				break;
			case GPX::pkind::AFTERVIDEO :
				cairo_set_source_rgb(cr, 0.3, 0.4, 0.6);
				break;
			case GPX::pkind::BEFORETRACE :
			case GPX::pkind::CURRENTVIDEO :
				cairo_set_source_rgb(cr, 1,1,1);
				break;
			}
		}
		if(first){
			first = false;
			cairo_move_to(cr, x, y);
		} else
			cairo_line_to(cr, x, y);
	}
	cairo_stroke(cr);
}

void PathGfx::drawGPMF(cairo_t *cr, int offset, uint32_t current){
	cairo_save(cr);
	if(current != (uint32_t)-1)
		cairo_set_source_rgb(cr, 0.11, 0.65, 0.88);

	for(uint32_t i = 0; i < this->video.getSampleCount(); i++){
		int x,y;

		this->posXY(this->video[i].getLatitude(), this->video[i].getLongitude(), x, y);
		x = this->off_x + (x-this->min_x) * this->scale + offset;
		y = this->SY - this->off_y - (y-this->min_y) * this->scale + offset;

		if(!i)
			cairo_move_to(cr, x, y);
		else
			cairo_line_to(cr, x, y);

		if(current == i){
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
		drawGPX(cr, 2);	// Shadow
		drawGPX(cr, 0);	// Path
	} else {
			/* Draw shadow */
		cairo_set_line_width(cr, 4);
		cairo_set_source_rgba(cr, 0,0,0, 0.55);
		this->drawGPMF(cr, 2);
	}

		/* Cleaning */
	cairo_destroy(cr);
}

void PathGfx::generateOneGfx( const char *fulltarget, char *filename, int index, GPMFdata &current ){
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
	this->drawGPMF(cr, 0, index);

	cairo_set_source_rgb(cr, 1,1,1);
	int x,y;
	this->posXY(current.getLatitude(), current.getLongitude(), x, y);
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
