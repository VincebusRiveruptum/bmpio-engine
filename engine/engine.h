#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H

#include "std.h"

/* EXTERNAL DEPENDENCIES */
#include "../deps/log/log.h"
#include "../deps/data/data.h"
#include "../deps/env/env.h"
#include "../hal/hal_inp.h"

/* VIDEO */
#include "../hal/hal_vid.h"

/* ENGINE */
#include "math.h"

/* SPACE */
#include "space.h"

/* ASSETS */
#include "assets.h"

/* TEST */
#include "test.h"

/* MEM */
#include "mem.h"

/* PLATFORM HARDWARE (DOS) */
#if defined(__WATCOMC__) || defined(__MSDOS__) || defined(DOS)
#include "../platform/dos/video/video.h"
#include "../platform/dos/video/vgaregs.h"
#endif

/* CONSTANTS =============================================================== */

#define ACTOR_FLAG_ACTIVE     0x01
#define ACTOR_FLAG_VISIBLE    0x02
#define ACTOR_FLAG_COLLIDABLE 0x04

/* TYPES =================================================================== */

typedef struct RenderQueue {
    struct List *renderList;
    struct List *renderOrder;
} RenderQueue;

/* VARS ==================================================================== */

extern unsigned long gameTicks;
extern unsigned long index;

extern Config *gameConfig;
extern Color *globalPalette;

/* PROTOTYPES ============================================================== */

void eng_renderFrame(unsigned long gametick);
void eng_setPalette(Color *palette);

#endif