/* Shared objects
 */

#ifndef SHARED_H
#define SHARED_H

#include <time.h>
#include <stdbool.h>

	/****
	 * Shared objects (as this file may be included everywhere :) 
	 ****/
extern bool verbose, debug, video;

	/* Generate video from given images sequence
	 * -> fulltarget, filename : full path and filename pointer
	 * -> iname : images' 3char name
	 * -> vname : target video name 
	 */
extern void generateVideo( const char *fulltarget, char *filename, const char *iname, const char *vname);

	/* print struct tm's content 
	 * CAUTION : doesn't issu a cariage return
	 */
extern void printtm( struct tm *t );

#endif
