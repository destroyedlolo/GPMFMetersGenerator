/* AltitudeGraphic
 * Generate altitude graphics
 *

The altitudes diverge between the phone and the GoPro GPS:
- first because the signal quality can be bad
- second, the GoPro may start recording video before all satellites are acquired
This can create a jump / break when switching from GPX to GoPro samples.

 */
#include "AltitudeGfx.h"

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <cmath>

AltitudeGfx::AltitudeGfx(GPVideo &v, GPX *h) : Gfx( 600,300, v, h ) {
	this->calcScales();
}

void AltitudeGfx::calcScales( void ){

		/* Label */
	cairo_text_extents_t extents;
	cairo_t *cr = cairo_create(this->background);

	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 35);
	cairo_text_extents(cr, "8888", &extents);
	this->offy = extents.height + 5;
	this->posLabel = this->SX - extents.x_advance - 55;

	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 27);
	cairo_text_extents(cr, "8888", &extents);
	this->offx = extents.x_advance + 10;

		/* Scales */
	if(this->hiking && this->hiking->isStory()){
		this->min_h = (((int)this->hiking->getMin().getAltitude())/50)*50;
		this->max_h = (((int)this->hiking->getMax().getAltitude())/50 + 1)*50;
		this->scale_w = (double)(this->SX - offx)/(double)this->hiking->getSampleCount();
	} else {
		this->min_h = (((int)this->video.getMin().getAltitude())/50)*50;
		this->max_h = (((int)this->video.getMax().getAltitude())/50 + 1)*50;
		this->scale_w = (double)(this->SX - offx)/(double)this->video.getSampleCount();
	}


	this->range_h = this->max_h - this->min_h;
	this->scale_h = (double)(this->SY - offy)/(double)this->range_h;
	this->delta_h = ((this->range_h/5)/50)*50;
	if(!this->delta_h)
		this->delta_h = this->range_h/5;

	if(this->hiking && this->hiking->isStory()){	// No offset here as we're not drawing a shadow
		this->soffx = this->offx + this->hiking->getCurrentStoryVideo().start * this->scale_w;
		this->sscale_w = (double)(this->hiking->getCurrentStoryVideo().end - this->hiking->getCurrentStoryVideo().start)/(double)this->video.getSampleCount() * this->scale_w;
	}

	cairo_destroy(cr);
}

void AltitudeGfx::drawGPX(cairo_t *cr, int offset){
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
		auto &p = this->hiking->getSamples()[idx];
		GPX::pkind kind = this->hiking->positionKind(idx);

		int x = this->offx + idx*this->scale_w + offset;
		int y = this->SY - (p.getAltitude() - this->min_h)*this->scale_h + offset;

		if(kind != prec){
			prec = kind;
			if(offset){	// shadow
				if(kind == GPX::pkind::CURRENTVIDEO){	// entering in the current video
					this->drawGPMF(cr, offset, (uint32_t)-2);
					for(; idx < (int)this->hiking->getSampleCount(); idx++)	// skiping
						if(this->hiking->positionKind(idx) != GPX::pkind::CURRENTVIDEO){
							idx--;
							break;
						} 
					continue;
				}
			} else {
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
					cairo_set_source_rgb(cr, 0.3, 0.4, 0.7);
					break;
				case GPX::pkind::BEFORETRACE :
					cairo_set_source_rgb(cr, 1,1,1);
					break;
				case GPX::pkind::CURRENTVIDEO :
					if(!first)	// draw previous gfx
						cairo_line_to(cr, x, y);
					for(; idx < (int)this->hiking->getSampleCount(); idx++)	// skiping
						if(this->hiking->positionKind(idx) != GPX::pkind::CURRENTVIDEO){
							idx--;
							break;
						}
					first = true;
					continue;
				}
			}
		}
		if(first){
			first = false;
			cairo_move_to(cr, x, y);
		} else
			cairo_line_to(cr, x, y);
	}

	if(offset)
		cairo_stroke_preserve(cr);
	else
		cairo_stroke(cr);
}

void AltitudeGfx::drawGPMF(cairo_t *cr, int offset, uint32_t current){
	if(current == (uint32_t)-1){	/* Drawing shadow */
		cairo_set_source_rgba(cr, 0,0,0, 0.55);
		cairo_set_line_width(cr, 5);
	} else if(current != (uint32_t)-2){	/* Drawing curve */
		cairo_set_line_width(cr, 3);
		cairo_set_source_rgb(cr, 0.11, 0.65, 0.88);	
	}

	if(this->hiking && this->hiking->isStory()){
		for(uint32_t i = 0; i < this->video.getSampleCount(); i++){
			int x = this->soffx + i*this->sscale_w + offset;
			int y = this->SY - (this->video[i].getAltitude() - this->min_h)*this->scale_h + offset;

			if(!i && current != uint32_t(-2))	/* First plot */
				cairo_move_to(cr, x, y);
			else
				cairo_line_to(cr, x, y);

			if(current == i){
				cairo_stroke(cr);
				cairo_move_to(cr, x, y);
				cairo_set_source_rgb(cr, 0.3, 0.5, 0.8);
			}
		}

		if(current != (uint32_t)-2)
			cairo_stroke(cr);
	} else {
		for(uint32_t i = 0; i < this->video.getSampleCount(); i++){
			int x = this->offx + i*this->scale_w + offset;
			int y = this->SY - (this->video[i].getAltitude() - this->min_h)*this->scale_h + offset;

			if(!i)	/* First plot */
				cairo_move_to(cr, x, y);
			else
				cairo_line_to(cr, x, y);

			if(current == i){
				cairo_stroke(cr);
				cairo_move_to(cr, x, y);
				cairo_set_source_rgb(cr, 1,1,1);
			}
		}

		if(current != uint32_t(-1))
			cairo_stroke(cr);
		else
			cairo_stroke_preserve(cr);
	}
}

void AltitudeGfx::generateBackground( void ){
	cairo_t *cr = cairo_create(this->background);

	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 27);
	cairo_set_source_rgba(cr, 1,1,1, 0.80);	/* Set white color */
	cairo_set_line_width(cr, 1);

	char t[8];
	for(int i = this->min_h; i <= this->max_h; i += this->delta_h){
		int y = this->SY - (i-this->min_h)*this->scale_h;
		sprintf(t, "%5d", i);

		cairo_move_to(cr, 3, y);
		cairo_show_text(cr, t);
		cairo_move_to(cr, this->offx, y);
		cairo_line_to(cr, this->SX, y);
	}
	cairo_stroke(cr);

		// Draw Shadow
	if(this->hiking && this->hiking->isStory())
		this->drawGPX(cr, 3);
	else
		this->drawGPMF(cr, 3);

	cairo_pattern_t *pat = cairo_pattern_create_linear(this->offx,this->SY - this->range_h*this->scale_h, this->offx,this->SY);
	cairo_pattern_add_color_stop_rgba(pat, 0, 0,0,0, 0.25);
	cairo_pattern_add_color_stop_rgba(pat, 1, 0,0,0, 0.05);
	cairo_set_source(cr, pat);

	cairo_line_to(cr, this->SX, this->SY);
	cairo_line_to(cr, this->offx, this->SY);
	cairo_line_to(cr, this->offx, this->SY - (this->video.getFirst().getAltitude() - this->min_h)*this->scale_h);

	cairo_fill(cr);

	if(this->hiking && this->hiking->isStory())
		drawGPX(cr, 0);

		/* Cleaning */
	cairo_pattern_destroy(pat);
	cairo_destroy(cr);
}

void AltitudeGfx::generateOneGfx(const char *fulltarget, char *filename, int index, GPMFdata &current){

		/*
		 * Initialise Cairo
		 */
	cairo_status_t err;

	cairo_surface_t *srf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, this->SX, this->SY);
	if(cairo_surface_status(srf) != CAIRO_STATUS_SUCCESS){
		puts("*F* Can't create Cairo's surface");
		exit(EXIT_FAILURE);
	}

	cairo_t *cr = cairo_create(srf);

	cairo_set_source_surface(cr, this->background, 0, 0);
	cairo_rectangle(cr, 0,0, this->SX, this->SY);
	cairo_fill(cr);
	cairo_stroke(cr);

		/* Draw Altitude curve */
	this->drawGPMF(cr, 0, index);

		/* Display the label */
	char t[8];
	sprintf(t, "%5d m", (int)current.getAltitude());
	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 35);

	cairo_set_source_rgba(cr, 0,0,0, 0.55);
	cairo_move_to(cr, this->posLabel+2, this->offy+2);
	cairo_show_text(cr, t);
	cairo_stroke(cr);

	cairo_set_source_rgb(cr, 1,1,1);	/* Set white color */
	cairo_move_to(cr, this->posLabel, this->offy);
	cairo_show_text(cr, t);
	cairo_stroke(cr);

		/* Display the spot */
	cairo_set_line_width(cr, 5);
	cairo_arc(cr, (this->soffx ? this->soffx : this->offx) + index*this->scale_w, this->SY - (current.getAltitude() - this->min_h)*this->scale_h , 8, 0, 2 * M_PI);
	cairo_stroke_preserve(cr);
	cairo_set_source_rgb(cr, 0.8, 0.2, 0.2);
	cairo_fill(cr);

		/* Writing the image */
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

void AltitudeGfx::GenerateAllGfx( const char *fulltarget, char *filename ){
	Gfx::GenerateAllGfx( fulltarget, filename );

		/* Generate video */
	if(genvideo)
		generateVideo(fulltarget, filename, "alt", "altitude");
}
