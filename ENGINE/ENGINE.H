#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include "STD.H"

// INTERNAL DEPENDENCES
#include "LOG/LOG.H"

// EXTERNAL DEPENDENCES
#include "../DEPS/DATA/DATA.H"
#include "../DEPS/ENV/ENV.H"

// PROTOTYPES
#include "PROTO.H"
#include "VIDEO.H"

// ENGINE
#include "MATH.H"
#include "INPUT.H"

// SPACE
#include "SPACE.H"

// ASSETS
#include "ASSETS.H"

// CONSTANTS ====================================================================
#define TR_ROTATION "ROTATION"
#define TR_TRANSLATION "TRANSLATION"
#define TR_SCALE "SCALE"

#define ACTOR_FLAG_ACTIVE     0x01
#define ACTOR_FLAG_VISIBLE    0x02
#define ACTOR_FLAG_COLLIDABLE 0x04

// PROTOYPES ====================================================================

void render2d(unsigned long gametick);

// TYPES ========================================================================
typedef struct Color{
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char i;
}Color;

extern unsigned long gameTicks;
extern unsigned long index;
extern SpriteTable *spriteTable;

typedef struct RenderQueue{
	struct List *renderList;
	struct List *renderOrder;
}RenderQueue;

// VARS ========================================================================

extern Config *gameConfig;
extern List *bmpList;
extern Color *globalPalette;

#endif