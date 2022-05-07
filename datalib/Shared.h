/* Shared objects
 */

#ifndef SHARED_H
#define SHARED_H

#include <stdbool.h>
#include <time.h>

	/****
	 * Shared objects (as this file may be included everywhere :) 
	 ****/
extern bool verbose, debug;

	/* print struct tm's content 
	 * CAUTION : doesn't issu a cariage return
	 */
extern void printtm( struct tm *t );

#endif
