/* GPVideo
 * 	Impersonates GoPro video
 */

#ifndef GPVIDEO_H
#define GPVIDEO_H

#include <cstdint>

class GPVideo {
	size_t mp4handle;

	uint32_t fr_num, fr_dem;	// Video framerates

protected:
	void readGPMF( void );

public:
		/* Read and parse a video */
	GPVideo( char * );
};
#endif
