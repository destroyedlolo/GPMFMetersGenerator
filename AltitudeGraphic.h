/* AltitudeGraphic
 * Generate altitude graphics
 *
 */

#ifndef ALTITUDE_H
#define ALTITUDE_H

#include "GPMFdata.h"

/* Generate altitude graphics
 * -> fulltarget : absolute target name (including directory)
 * -> filename : pointer to the filename only
 * -> index : which is the current index  (-1 : none)
 */
extern void GenerateAllAltitudeGfx( const char *fulltarget, char *filename );

#endif
