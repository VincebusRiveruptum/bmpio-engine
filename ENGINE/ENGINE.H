#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include "STD.H"

// EXTERNAL DEPENDENCES
#include "../DEPS/LOG/LOG.H"
#include "../DEPS/DATA/DATA.H"
#include "../DEPS/ENV/ENV.H"
#include "../DEPS/INPUT/INPUT.H"

// VIDEO
#include "VIDEO.H"

// ENGINE
#include "MATH.H"

// SPACE
#include "SPACE.H"

// ASSETS
#include "ASSETS.H"

// TEST
#include "TEST.H"

// CONSTANTS ====================================================================

#define TR_ROTATION "ROTATION"
#define TR_TRANSLATION "TRANSLATION"
#define TR_SCALE "SCALE"

#define ACTOR_FLAG_ACTIVE     0x01
#define ACTOR_FLAG_VISIBLE    0x02
#define ACTOR_FLAG_COLLIDABLE 0x04

// TYPES ========================================================================

typedef struct RenderQueue{
	struct List *renderList;
	struct List *renderOrder;
}RenderQueue;

// VARS ========================================================================

extern unsigned long gameTicks;
extern unsigned long index;

extern Config *gameConfig;
extern Color *globalPalette;

// PROTOYPES ====================================================================

void eng_render2d(unsigned long gametick);
bool eng_checkConfig();
void eng_setPalette(Color *palette);

#endif