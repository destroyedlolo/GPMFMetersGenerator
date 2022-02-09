/* SpeedGraphic
 * Generate speed-o-meter graphics
 *
 */

#ifndef SPEED_H
#define SPEED_H

#include "GPMFdata.h"

#include "Shared.h"

extern bool s3d;
extern bool sboth;

/* Generate speed graphics
 * -> fulltarget : absolute target name (including directory)
 * -> filename : pointer to the filename only
 * -> index : which is the current index
 */
extern void GenerateAllSpeedGfx( const char *fulltarget, char *filename );
extern void GenerateSpeedGfx( const char *fulltarget, char *filename, int index, struct GPMFdata *);

#endif
