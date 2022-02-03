/* SpeedTracker
 * Generate speed tracker graphics
 *
 * Notez-bien : speed tracker is supposed to be a companion of speed-o-meter
 * 	consequently, only the trend is displayed, not value.
 */

#ifndef SPD_TRK_H
#define SPD_TRK_H

#include "SpeedGraphic.h"

/* Generate speed tracker graphics
 * -> fulltarget : absolute target name (including directory)
 * -> filename : pointer to the filename only
 * -> index : which is the current index
 */
extern void GenerateAllSpeedTrkGfx( const char *fulltarget, char *filename );
extern void GenerateSpeedTrkGfx( const char *fulltarget, char *filename, int index, struct GPMFdata *);

#endif
