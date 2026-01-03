
#ifndef ENGINE_MEM_H
#define ENGINE_MEM_H

#include "STD.H"
#include "../DEPS/LOG/LOG.H"

#define MEM_ARENA_PERMANENT 0
#define MEM_ARENA_CACHE 1
#define MEM_ARENA_PURGABLE 2

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

/* PROTOTYPES ===========================================================================*/

void mem_arena_init(MemoryArena *arena, char *name, unsigned char type, size_t size);
void *mem_arena_alloc(MemoryArena *arena, size_t size);
void mem_arena_reset(MemoryArena *arena);
void mem_arena_free(MemoryArena *arena);

#endif