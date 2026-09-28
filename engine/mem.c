#include "mem.h"
#include "assets.h"
#include "engine.h"
#include "game.h"
#include "space.h"

#ifdef STANDALONE
// Stub to satisfy LOG.C dependencies in standalone mode
bool eng_checkConfig() {
    return false; // Default to standard logging
}
#endif

// Global pointers
MemoryArena *gameSessionArena = NULL;
MemoryArena *sceneArena = NULL;
MemoryArena *frameArena = NULL;
MemoryArena *testArena = NULL;

void mem_init() {
    gameSessionArena = (MemoryArena*)malloc(sizeof(MemoryArena));
    sceneArena       = (MemoryArena*)malloc(sizeof(MemoryArena));
    frameArena       = (MemoryArena*)malloc(sizeof(MemoryArena));
    testArena        = (MemoryArena*)malloc(sizeof(MemoryArena));

    if(!gameSessionArena || !sceneArena || !frameArena || !testArena){
        printf("\n[mem_init]: FATAL ERROR: Could not allocate memory for arena control structures.");
        exit(1);
    }

    mem_arena_init(gameSessionArena, "Session", ARENA_SIZE_SESSION);
    mem_arena_init(sceneArena,       "Scene",   ARENA_SIZE_SCENE);
    mem_arena_init(frameArena,       "Frame",   ARENA_SIZE_FRAME);
    mem_arena_init(testArena,        "Test",    ARENA_SIZE_TEST);
}

void mem_shutdown() {
    mem_arena_free(gameSessionArena);
    mem_arena_free(sceneArena);
    mem_arena_free(frameArena);
    mem_arena_free(testArena);
    
    free(gameSessionArena);
    free(sceneArena);
    free(frameArena);
    free(testArena);
}

#ifdef STANDALONE

int main(int argc, char *argv[]){
    size_t type_size;
    char *module_name = NULL;

    if(argc < 2){
        printf("\nUsage: %s [MODULE_NAME]\n", argv[0]);
        printf("\nOptions:\n");
        printf("\tASSETS, ENGINE, GAME, MEM, SPACE\n");
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