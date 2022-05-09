/* GPVideo
 * 	Impersonates GoPro video
 */

#ifndef GPVIDEO_H
#define GPVIDEO_H

#include <cstdint>

class GPVideo {
	uint32_t fr_num, fr_dem;	// Video framerates
};

#endif
