#include "Context.h"

#include <cstdio>

bool verbose = false;
bool debug = false;

void printtm( struct tm *t ){
	printf( "%4d-%02d-%02d %02d:%02d:%02d ", 
		t->tm_year+1900, t->tm_mon+1, t->tm_mday,
		t->tm_hour, t->tm_min, t->tm_sec
	);
	if(t->tm_gmtoff)
		printf("(offset %6ld sec)", t->tm_gmtoff);
}

unsigned char char2int( const char *p ){
	unsigned char ret = (*p - '0')*10;
	ret += *(++p) - '0';
	return ret;
}
