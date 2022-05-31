/* Generate trekking statistic "graphic"
 */

#include "TrkStatGfx.h"

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>

TrekkingStatGfx::TrekkingStatGfx(GPVideo &v, GPX *h) : Gfx( 300,100, v, h ) {
	this->calcScales();
}

void TrekkingStatGfx::calcScales( void ){
	cairo_text_extents_t extents;
	cairo_t *cr = cairo_create(this->background);

	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 35);
	cairo_text_extents(cr, "8888.888 km", &extents);
	this->dst_x = this->SX - extents.x_advance + 5;	// used only for the icon
	this->dst_y = extents.height + 5;

	cairo_destroy(cr);
}

void TrekkingStatGfx::generateBackground( void ){
}

void TrekkingStatGfx::generateOneGfx(const char *fulltarget, char *filename, int index, GPMFdata &current){
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

	cairo_text_extents_t extents;
	char t[16];
	sprintf(t, "%8.3f km", current.getCumulativeDistance()/*/100*/);
	cairo_select_font_face(cr, "sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 35);
	cairo_text_extents(cr, t, &extents);

	cairo_set_source_rgba(cr, 0,0,0, 0.55);
	cairo_move_to(cr, (this->SX - extents.x_advance), this->dst_y+2);
	cairo_show_text(cr, t);
	cairo_stroke(cr);

	cairo_set_source_rgb(cr, 1,1,1);	/* Set white color */
	cairo_move_to(cr, (this->SX - extents.x_advance)-2, this->dst_y);
	cairo_show_text(cr, t);
	cairo_stroke(cr);

		/* Writing the image */
	sprintf(filename, "tstt%07d.png", index);
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

void TrekkingStatGfx::GenerateAllGfx( const char *fulltarget, char *filename ){
	Gfx::GenerateAllGfx( fulltarget, filename );

		/* Generate video */
	if(genvideo)
		generateVideo(fulltarget, filename, "tstt", "TrekkingStat");
}
