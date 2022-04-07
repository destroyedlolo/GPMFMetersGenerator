/* PathGraphic
 * Generate simple path graphics
 *
 */

#ifndef PATH_H
#define PATH_H

#include "GPMFdata.h"

/* Generate path graphics
 * -> fulltarget : absolute target name (including directory)
 * -> filename : pointer to the filename only
 * -> index : which is the current index
 */
extern void GenerateAllPathGfx( const char *fulltarget, char *filename );

#endif
