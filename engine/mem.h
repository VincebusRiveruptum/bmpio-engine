#ifndef ENGINE_MEM_H
#define ENGINE_MEM_H

#include "std.h"
#include "../deps/log/log.h"
#include "../deps/mem/mem.h"

struct MemoryArena;

/* ARENA SIZES ============================================================= */

#define ARENA_SIZE_SESSION  (4096 * 1024)   /* 4MB for high-level assets */
#define ARENA_SIZE_SCENE    (1024 * 1024)   /* 1MB for level-specific data */
#define ARENA_SIZE_FRAME    (128 * 1024)    /* 128KB for transients */
#define ARENA_SIZE_TEST     (512 * 1024)    /* 512KB for engineering tests */

/* GLOBAL VARS ============================================================ */

extern struct MemoryArena *mem_gameSessionArena;
extern struct MemoryArena *mem_sceneArena;
extern struct MemoryArena *mem_frameArena;
extern struct MemoryArena *mem_testArena;

#define gameSessionArena mem_gameSessionArena
#define sceneArena mem_sceneArena
#define frameArena mem_frameArena
#define testArena mem_testArena

/* PROTOTYPES ============================================================== */

void mem_init(void);
void mem_shutdown(void);

#endif