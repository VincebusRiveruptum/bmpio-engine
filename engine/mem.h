
#ifndef ENGINE_MEM_H
#define ENGINE_MEM_H

#include "std.h"
#include "../deps/log/log.h"
#include "../deps/mem/mem.h"

struct MemoryArena;
/* ARENA SIZES =========================================================================*/

#define ARENA_SIZE_SESSION  (4096 * 1024)   // 4MB for high-level assets
#define ARENA_SIZE_SCENE    (1024 * 1024)   // 1MB for level-specific data
#define ARENA_SIZE_FRAME    (128 * 1024)    // 128KB for transients (reset every frame)
#define ARENA_SIZE_TEST     (512 * 1024)    // 512KB for internal engineering tests

/* GLOBAL VARS ===========================================================================*/

extern struct MemoryArena *gameSessionArena;
extern struct MemoryArena *sceneArena;
extern struct MemoryArena *frameArena;
extern struct MemoryArena *testArena;

/* PROTOTYPES ===========================================================================*/

void mem_init();
void mem_shutdown();

#endif