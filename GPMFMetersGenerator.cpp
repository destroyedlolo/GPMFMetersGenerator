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
#include <unistd.h>

bool verbose = false;
bool debug = false;

	/* Configuration */

#define VERSION "2.00a01"

int main(int argc, char *argv[]){
	int opt;

		/* Reading arguments */
	while(( opt = getopt(argc, argv, ":vdh")) != -1) {
		switch(opt){
		case 'd':	// debug implies verbose
			debug = true;
		case 'v':
			verbose = true;
			break;
		case '?':	// unknown option
			printf("unknown option: -%c\n", optopt);
		case 'h':
		case ':':	// no argument provided
			puts(
				"\nGPMFMetersGenerator v" VERSION "\n"
				"(c) L.Faillie (destroyedlolo) 2022\n"
				"\nKnown opts :\n"
				"-v : turn verbose on\n"
				"-d : turn debugging messages on\n"
			);
			break;
		}
	}

		/* And videos */
	for(; optind < argc; optind++){
		if(verbose)
			printf("*I Reading '%s'\n", argv[optind]);
	}
}
