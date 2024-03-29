/* Context
 * Shared (among all tools and libraries) context configuration
 */

#ifndef CONTEXT_H
#define CONTEXT_H

#include <cstdio>


extern bool verbose;
extern bool debug;

extern bool enfquality;

	/* Utilities */

#include <ctime>

/* print the content of a tm */
extern void printtm( struct tm *, FILE *f=NULL );

/* Convert 2 chars to int */
extern unsigned char char2int( const char * );

#endif
