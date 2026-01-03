
#ifndef ENGINE_MEM_H
#define ENGINE_MEM_H

#include "STD.H"
#include "../DEPS/LOG/LOG.H"

#define MEM_ARENA_PERMANENT 0
#define MEM_ARENA_CACHE 1
#define MEM_ARENA_PURGABLE 2

/* ARENA SIZES =========================================================================*/

#define ARENA_SIZE_SESSION  (4096 * 1024)   // 4MB for high-level assets
#define ARENA_SIZE_SCENE    (1024 * 1024)   // 1MB for level-specific data
#define ARENA_SIZE_FRAME    (128 * 1024)    // 128KB for transients (reset every frame)
#define ARENA_SIZE_TEST     (512 * 1024)    // 512KB for internal engineering tests

/* TYPES ===========================================================================*/

typedef struct MemoryArena{
    char name[32];
    unsigned char type;
    void *base;
    size_t size;
    size_t offset;
} MemoryArena;

/* GLOBAL VARS ===========================================================================*/

extern MemoryArena *gameSessionArena;
extern MemoryArena *sceneArena;
extern MemoryArena *frameArena;
extern MemoryArena *testArena;

/* PROTOTYPES ===========================================================================*/

void mem_init();
void mem_shutdown();

void mem_arena_init(MemoryArena *arena, char *name, unsigned char type, size_t size);
void *mem_arena_alloc(MemoryArena *arena, size_t size);
void mem_arena_reset(MemoryArena *arena);
void mem_arena_free(MemoryArena *arena);

#endif