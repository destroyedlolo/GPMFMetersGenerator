/* GPMFMetersGenerator
 *
 * Generate images sequence from GPMF' meters
 *
 * 26/01/2022 - Starting development
 * 28/02/2022 - Handle multipart video
 * 07/03/2022 - Add GPX generation
 * 16/04/2022 - Bump to v1.0 (CAUTION, verbose and debug options changed !)
 *
 * 09/05/2022 - Switch to v2.0
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <libgen.h>

#include "datalib/Context.h"
#include "datalib/GPVideo.h"
#include "datalib/GPX.h"

#include "gfxlib/SpeedGfx.h"
#include "gfxlib/AltitudeGfx.h"
#include "gfxlib/PathGfx.h"
#include "gfxlib/SpeedTrkGfx.h"
#include "gfxlib/Export.h"

	/* Configuration */

#define VERSION "2.01a01"

	/* Which gfx to generate */
static char gfx_speed = 0;	/* 0,2,3,b */
static bool gfx_altitude = false;
static bool gfx_path = false;
static char gfx_strk = 0;	/* 0,2,3 */
static bool gfx_GPX = false;
static bool gfx_KML = false;

GPX *journey = NULL;	// full journey trace

int main(int argc, char *argv[]){
	bool force = false;

		/* Reading arguments */
	int opt;
	while(( opt = getopt(argc, argv, ":vdhFs:apk:VXKG:")) != -1) {
		switch(opt){
		case 'F':
			force = true;
			break;
		case 'd':	// debug implies verbose
			debug = true;
		case 'v':
			verbose = true;
			break;
		case 'V':
			genvideo = false;
			break;
		case 's':
			switch(*optarg){
			case '2':
			case '3':
			case 'b':
				gfx_speed = *optarg;
				break;
			default :
				puts("*E* Only 2, 3, b are recognized as -s option");
				exit(EXIT_FAILURE);
			}
			break;
		case 'a':
			gfx_altitude = true;
			break;
		case 'p':
			gfx_path = true;
			break;
		case 'k':
			switch(*optarg){
			case '2':
			case '3':
				gfx_strk = *optarg;
				break;
			default :
				puts("*E* Only 2, 3 are recognized as -k option");
				exit(EXIT_FAILURE);
			}
			break;
		case 'X':
			gfx_GPX = true;
			break;
		case 'K':
			gfx_KML = true;
			break;
		case 'G':
			journey = new GPX(optarg);
			journey->Dump();
			break;
		case '?':	// unknown option
			printf("unknown option: -%c\n", optopt);
		case 'h':
		case ':':	// no argument provided (or missing argument)
			if(optopt == 's'){
				gfx_speed = '2';
				break;
			} else if(optopt == 's'){
				gfx_strk = '2';
				break;
			}
	
			puts(
				"\nGPMFMetersGenerator v" VERSION "\n"
				"(c) L.Faillie (destroyedlolo) 2022\n"
				"\nKnown opts :\n"
				"-s[2|3|b] : enable speed gfx (default 2d, 3: 3d, b: both)\n"
				"-k[2|3] : enable speed tracker gfx (default 2d, 3: 3d)\n"
				"-a : enable altitude gfx\n"
				"-p : enable path gfx\n"
				"-X : export telemetry as GPX file\n"
				"-K : export telemetry as KML file\n"
				"\n"
				"-G<file> : load a GPX file\n"
				"\n"
				"-V : Don't generate video, keep PNG files\n"
				"-F : don't fail if the target directory exists\n"
				"-v : turn verbose on\n"
				"-d : turn debugging messages on\n"
			);
			exit(EXIT_FAILURE);
			break;
		}
	}

		/* Handle first videos */
	if(optind >= argc){
		puts("*F* No Video provided");
		exit(EXIT_FAILURE);		
	}

		/* Determine target directory name from the 1st video */
	size_t len = strlen(argv[optind]) + 4;
	if( strlen("img0123456.png") > len )
		len = strlen("img0123456.png");
	len += strlen(argv[optind]) + 1;
	char targetDir[ len ];						/* Where files will be created */
	strcpy(targetDir, argv[optind]);
	char *targetFile = strrchr(targetDir, '.');	/* target file name */
	if(!targetFile){
		puts("*F* Video filename doesn't have extension");
		exit(EXIT_FAILURE);
	}


		/* Create the target directory */
	*targetFile = 0;
	if(!force){
		DIR* dir = opendir(targetDir);
		if(dir){
			closedir(dir);
			printf("*F* '%s' alreay exists\n", targetDir);
			exit(EXIT_FAILURE);
		} else if(errno != ENOENT){
			perror(targetDir);
			exit(EXIT_FAILURE);
		}
	}

	if(mkdir(targetDir, S_IRWXU | S_IRWXG | S_IRWXO) == -1){
		if((verbose) && !force)
			perror(targetDir);
		if(!force)
			exit(EXIT_FAILURE);
	}

		/* Keep video name */
	char *fname = basename(targetDir);
	char vname[strlen(fname) + 1];
	strcpy(vname, fname);

		/* done with directory */
	*(targetFile++) = '/';
	*targetFile = 0;
	if(verbose)
		printf("*I* images will be generated in '%s'\n", targetDir);

		/* Read the 1st video */
	if(verbose)
		printf("*I Reading '%s'\n", argv[optind]);
	GPVideo video(argv[optind++]);

	for(; optind < argc; optind++){
		if(verbose)
			printf("*I Reading '%s'\n", argv[optind]);

		video.AddPart( argv[optind] );
	}
	video.Dump();

		/* Generate videos */
	if(gfx_speed){
		SpeedGfx gfx( video, gfx_speed );
		gfx.GenerateAllGfx(targetDir, targetFile);
	}

	if(gfx_altitude){
		AltitudeGfx gfx( video );
		gfx.GenerateAllGfx(targetDir, targetFile);
	}

	if(gfx_path){
		PathGfx gfx( video );
		gfx.GenerateAllGfx(targetDir, targetFile);
	}

	if(gfx_strk){
		SpeedTrkGfx gfx( video, gfx_strk );
		gfx.GenerateAllGfx(targetDir, targetFile);
	}

	if(gfx_GPX){
		Export gfx( video );
		gfx.generateGPX(targetDir, targetFile, vname);
	}

	if(gfx_KML){
		Export gfx( video );
		gfx.generateKML(targetDir, targetFile, vname);
	}
}
