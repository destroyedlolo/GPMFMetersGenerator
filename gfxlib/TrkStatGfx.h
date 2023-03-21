/* Generate trekking statistic "graphic"
 */

#ifndef TRKSTTGFX_H
#define TRKSTTGFX_H

#include "Gfx.h"

class TrekkingStatGfx : public Gfx {
public:
	enum gfxtype { NONE=0, HM=1, HMS=2 };

private:
	gfxtype type;

	long sDistance;		// Original distance covered by the story
	time_t beginning;	// Beginning of the journey

	int dst_x, dst_y;
	int duration_x, duration_y;

protected:
	void calcScales( void );
	void generateBackground( void );
	void generateOneGfx( const char *, char *, int , GPMFdata & );

public:
	TrekkingStatGfx(GPVideo &v, GPX *, gfxtype);

	void GenerateAllGfx( const char *dir, char *file );
};

#endif
