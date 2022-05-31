/* Generate trekking statistic "graphic"
 */

#ifndef TRKSTTGFX_H
#define TRKSTTGFX_H

#include "Gfx.h"

class TrekkingStatGfx : public Gfx {
	long Distance;
	int dst_x, dst_y;

protected:
	void calcScales( void );
	void generateBackground( void );
	void generateOneGfx( const char *, char *, int , GPMFdata & );

public:
	TrekkingStatGfx(GPVideo &v, GPX *h);

	void GenerateAllGfx( const char *dir, char *file );
};

#endif
