/* Gfx
 *
 * mother class for all graphics generation
 */

#include "Gfx.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>

bool genvideo = true;

void Gfx::generateBackGround( void ){
	this->background = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, this->SX, this->SY);
    if(cairo_surface_status(background) != CAIRO_STATUS_SUCCESS){
        puts("*F* Can't create Cairo's surface");
        exit(EXIT_FAILURE);
    }
}

void Gfx::generateVideo( const char *fulltarget, char *filename, const char *iname, const char *vname ){
	*filename = 0;

	static const char *cmd = "ffmpeg -y -framerate 9 -i %s%s%%07d.png -vcodec png %s%s.mov";
	char buf[ strlen(cmd) + 2*strlen(fulltarget) + strlen(iname) + 7 ];	// as '%?' are not subtracted so some security room left
	sprintf(buf, "ffmpeg -y -framerate 9 -i %s%s%%07d.png -vcodec png %s%s.mov",
		fulltarget, iname, fulltarget, vname
	);

	int ret = system(buf);
	if(ret){
		printf("*E* ffmpeg returned %d error code\n", ret);
		return;
	}

		/* Remove .png */
	DIR *dp;
	struct dirent *entry;

	if(!(dp = opendir(fulltarget))){
        printf("*F* cannot open directory: %s\n", fulltarget);
        exit(EXIT_FAILURE);
    }

	const size_t len = strlen(iname);
	while((entry = readdir(dp))){
		if(!strncmp(iname, entry->d_name, len)){
			size_t l = strlen(entry->d_name);
			if( l>4 && !strcmp(entry->d_name + l - 4, ".png") ){
				sprintf(buf, "%s%s", fulltarget, entry->d_name);
				remove(buf);
			}
		} 
	}

	closedir(dp);
}
