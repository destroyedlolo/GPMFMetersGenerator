/* AltitudeGraphic
 * Generate altitude graphics
 */
#include "AltitudeGfx.h"

#include <cstdio>
#include <cstdlib>

AltitudeGfx::AltitudeGfx(GPVideo &v) : Gfx( 600,300, v ) {
	this->calcScales();
}

void AltitudeGfx::calcScales( void ){
}

void AltitudeGfx::generateBackGround( ){
}

void AltitudeGfx::generateOneGfx(const char *fulltarget, char *filename, int index, struct GPMFdata *current){
}

void AltitudeGfx::GenerateAllGfx( const char *fulltarget, char *filename ){
}

