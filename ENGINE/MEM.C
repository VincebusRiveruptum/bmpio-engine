#include "MEM.H"
#include "ASSETS.H"
#include "ENGINE.H"
#include "GAME.H"
#include "SPACE.H"

#ifdef STANDALONE
// Stub to satisfy LOG.C dependencies in standalone mode
bool eng_checkConfig() {
    return false; // Default to standard logging
}
#endif

MemoryArena *gameSessionArena = NULL;
MemoryArena *sceneArena = NULL;
MemoryArena *frameArena = NULL;

void mem_arena_init(MemoryArena *arena, char *name, unsigned char type, size_t size){
    if(!arena) return;

    arena->base = malloc(size);
    if(!arena->base){
        logger("\n[mem_arena_init]: Error: Could not allocate %d bytes for arena %s", size, name);
        return;
    }

    arena->size = size;
    arena->offset = 0;
    arena->type = type;
    strncpy(arena->name, name, 31);
    
    logger("\n[mem_arena_init]: Arena %s initialized with %d bytes", name, size);
}

void *mem_arena_alloc(MemoryArena *arena, size_t size){
    void *ptr = NULL;

    if(!arena || !arena->base){
        logger("\n[mem_arena_alloc]: Error: Arena is not initialized");
        return NULL;
    }

    // Align to 4 bytes for 32-bit performance
    size = (size + 3) & ~3;

    if(arena->offset + size > arena->size){
        logger("\n[mem_arena_alloc]: Error: Arena %s out of memory (request: %d, left: %d)", 
               arena->name, size, arena->size - arena->offset);
        return NULL;
    }

    ptr = (void *)((unsigned char *)arena->base + arena->offset);
    arena->offset += size;

    return ptr;
}

void mem_arena_reset(MemoryArena *arena){
    if(arena){
        arena->offset = 0;
    }
}

void mem_arena_free(MemoryArena *arena){
    if(arena && arena->base){
        logger("\n[mem_arena_free]: Freeing arena %s", arena->name);
        free(arena->base);
        arena->base = NULL;
        arena->offset = 0;
        arena->size = 0;
    }
}

#ifdef STANDALONE

int main(int argc, char *argv[]){
    size_t type_size;
    char *module_name = NULL;

    if(argc == 0){
        printf("\nUsage: %s [options]\n", argv[0]);
        printf("\nOptions:\n");
        printf("\tMODULE_NAME\tDisplay a module structures size\n");
        return 0;
    }

    printf("\nMemory Arena Testing module\n");
    printf("\nThis will display list of known structures and their sizes\n");
    
    module_name = argv[1];

    if(!module_name || strcmp(module_name, "") == 0){
        printf("\nUsage: %s [options]\n", argv[0]);
        printf("\nOptions:\n");
        printf("\tMODULE_NAME\tDisplay a module structures size\n");
        return 0;
    }

    if(strcmp(module_name, "ASSETS") == 0){
        printf("\nASSETS.H\n");
        printf("\n========\n");
        
        type_size = sizeof(Color);
        printf("\nstruct Color: %d bytes", type_size);
        
        type_size = sizeof(FileHeader);
        printf("\nstruct FileHeader: %d bytes", type_size);
        
        type_size = sizeof(InfoHeader);
        printf("\nstruct InfoHeader: %d bytes", type_size);
        
        type_size = sizeof(BMPfile);
        printf("\nstruct BMPfile: %d bytes", type_size);
        
        type_size = sizeof(BMPdata);
        printf("\nstruct BMPdata: %d bytes", type_size);
        
        type_size = sizeof(Sprite);
        printf("\nstruct Sprite: %d bytes", type_size);
        
        type_size = sizeof(Animation);
        printf("\nstruct Animation: %d bytes", type_size);
        
        type_size = sizeof(RotationTransformation);
        printf("\nstruct RotationTransformation: %d bytes", type_size);
        
        type_size = sizeof(TranslationTransformation);
        printf("\nstruct TranslationTransformation: %d bytes", type_size);
        
        type_size = sizeof(Transformation);
        printf("\nstruct Transformation: %d bytes", type_size);
    }

    if(strcmp(module_name, "ENGINE") == 0){
        printf("\nENGINE.H\n");
        printf("\n========\n");
        
        type_size = sizeof(RenderQueue);
        printf("\nstruct RenderQueue: %d bytes", type_size);
    }

    if(strcmp(module_name, "GAME") == 0){
        printf("\nGAME.H\n");
        printf("\n========\n");
        
        type_size = sizeof(Stats);
        printf("\nstruct Stats: %d bytes", type_size);
        
        type_size = sizeof(Collision);
        printf("\nstruct Collision: %d bytes", type_size);

        type_size = sizeof(Action);
        printf("\nstruct Action: %d bytes", type_size);

        type_size = sizeof(Actor);
        printf("\nstruct Actor: %d bytes", type_size);

        type_size = sizeof(Asset);
        printf("\nstruct Asset: %d bytes", type_size);

        type_size = sizeof(AssetList);
        printf("\nstruct AssetList: %d bytes", type_size);
    }

    if(strcmp(module_name, "MEM") == 0){
        printf("\nMEM.H\n");
        printf("\n========\n");
        
        type_size = sizeof(MemoryArena);
        printf("\nstruct MemoryArena: %d bytes", type_size);
    }
    if(strcmp(module_name, "SPACE") == 0){
        printf("\nSPACE.H\n");
        printf("\n========\n");
        
        type_size = sizeof(Coordinates);
        printf("\nstruct Coordinates: %d bytes", type_size);

        type_size = sizeof(ScreenCoordinates);
        printf("\nstruct ScreenCoordinates: %d bytes", type_size);

        type_size = sizeof(Camera);
        printf("\nstruct Camera: %d bytes", type_size);

    }

    return 0;
}
    
#endif