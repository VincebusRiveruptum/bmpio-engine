#include "mem.h"
#include "assets.h"
#include "engine.h"
#include "game.h"
#include "space.h"

#ifdef STANDALONE
/* Stub to satisfy LOG.C dependencies in standalone mode */
bool eng_checkConfig(void)
{
    return false;
}
#endif

/* Global pointers */
MemoryArena *mem_gameSessionArena = NULL;
MemoryArena *mem_sceneArena = NULL;
MemoryArena *mem_frameArena = NULL;
MemoryArena *mem_testArena = NULL;

void mem_init(void)
{
    mem_gameSessionArena = (MemoryArena *)malloc(sizeof(MemoryArena));
    mem_sceneArena       = (MemoryArena *)malloc(sizeof(MemoryArena));
    mem_frameArena       = (MemoryArena *)malloc(sizeof(MemoryArena));
    mem_testArena        = (MemoryArena *)malloc(sizeof(MemoryArena));

    if (!mem_gameSessionArena ||
        !mem_sceneArena ||
        !mem_frameArena ||
        !mem_testArena) {
        printf(
            "\n[mem_init]: FATAL ERROR: "
            "Could not allocate memory for arena structures."
        );
        exit(1);
    }

    mem_arena_init(mem_gameSessionArena, "Session", ARENA_SIZE_SESSION);
    mem_arena_init(mem_sceneArena,       "Scene",   ARENA_SIZE_SCENE);
    mem_arena_init(mem_frameArena,       "Frame",   ARENA_SIZE_FRAME);
    mem_arena_init(mem_testArena,        "Test",    ARENA_SIZE_TEST);
}

void mem_shutdown(void)
{
    mem_arena_free(mem_gameSessionArena);
    mem_arena_free(mem_sceneArena);
    mem_arena_free(mem_frameArena);
    mem_arena_free(mem_testArena);

    free(mem_gameSessionArena);
    free(mem_sceneArena);
    free(mem_frameArena);
    free(mem_testArena);

    mem_gameSessionArena = NULL;
    mem_sceneArena = NULL;
    mem_frameArena = NULL;
    mem_testArena = NULL;
}

#ifdef STANDALONE

int main(int argc, char *argv[])
{
    size_t type_size = 0;
    char *module_name = NULL;

    if (argc < 2) {
        printf("\nUsage: %s [MODULE_NAME]\n", argv[0]);
        printf("\nOptions:\n");
        printf("\tASSETS, ENGINE, GAME, MEM, SPACE\n");
        return 0;
    }

    printf("\nMemory Arena Testing module\n");
    printf("\nThis will display list of known structures and their sizes\n");

    module_name = argv[1];

    if (!module_name || strcmp(module_name, "") == 0) {
        printf("\nUsage: %s [options]\n", argv[0]);
        printf("\nOptions:\n");
        printf("\tMODULE_NAME\tDisplay a module structures size\n");
        return 0;
    }

    if (strcmp(module_name, "ASSETS") == 0) {
        printf("\nASSETS.H\n");
        printf("\n========\n");

        type_size = sizeof(Color);
        printf("\nstruct Color: %u bytes", (unsigned int)type_size);

        type_size = sizeof(FileHeader);
        printf("\nstruct FileHeader: %u bytes", (unsigned int)type_size);

        type_size = sizeof(InfoHeader);
        printf("\nstruct InfoHeader: %u bytes", (unsigned int)type_size);

        type_size = sizeof(BMPfile);
        printf("\nstruct BMPfile: %u bytes", (unsigned int)type_size);

        type_size = sizeof(BMPdata);
        printf("\nstruct BMPdata: %u bytes", (unsigned int)type_size);

        type_size = sizeof(Sprite);
        printf("\nstruct Sprite: %u bytes", (unsigned int)type_size);

        type_size = sizeof(Animation);
        printf("\nstruct Animation: %u bytes", (unsigned int)type_size);

        type_size = sizeof(RotationTransformation);
        printf(
            "\nstruct RotationTransformation: %u bytes",
            (unsigned int)type_size
        );

        type_size = sizeof(TranslationTransformation);
        printf(
            "\nstruct TranslationTransformation: %u bytes",
            (unsigned int)type_size
        );

        type_size = sizeof(Transformation);
        printf(
            "\nstruct Transformation: %u bytes",
            (unsigned int)type_size
        );
    }

    if (strcmp(module_name, "ENGINE") == 0) {
        printf("\nENGINE.H\n");
        printf("\n========\n");

        type_size = sizeof(RenderQueue);
        printf("\nstruct RenderQueue: %u bytes", (unsigned int)type_size);
    }

    if (strcmp(module_name, "GAME") == 0) {
        printf("\nGAME.H\n");
        printf("\n========\n");

        type_size = sizeof(Stats);
        printf("\nstruct Stats: %u bytes", (unsigned int)type_size);

        type_size = sizeof(Collision);
        printf("\nstruct Collision: %u bytes", (unsigned int)type_size);

        type_size = sizeof(Action);
        printf("\nstruct Action: %u bytes", (unsigned int)type_size);

        type_size = sizeof(Asset);
        printf("\nstruct Asset: %u bytes", (unsigned int)type_size);

        type_size = sizeof(Actor);
        printf("\nstruct Actor: %u bytes", (unsigned int)type_size);

        type_size = sizeof(AssetList);
        printf("\nstruct AssetList: %u bytes", (unsigned int)type_size);
    }

    if (strcmp(module_name, "MEM") == 0) {
        printf("\nMEM.H\n");
        printf("\n========\n");

        type_size = sizeof(MemoryArena);
        printf("\nstruct MemoryArena: %u bytes", (unsigned int)type_size);
    }

    if (strcmp(module_name, "SPACE") == 0) {
        printf("\nSPACE.H\n");
        printf("\n========\n");

        type_size = sizeof(Coordinates);
        printf("\nstruct Coordinates: %u bytes", (unsigned int)type_size);

        type_size = sizeof(ScreenCoordinates);
        printf("\nstruct ScreenCoordinates: %u bytes", (unsigned int)type_size);

        type_size = sizeof(Camera);
        printf("\nstruct Camera: %u bytes", (unsigned int)type_size);
    }

    return 0;
}

#endif
