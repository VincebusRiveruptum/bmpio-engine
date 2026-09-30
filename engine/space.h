#ifndef ENGINE_SPACE_H
#define ENGINE_SPACE_H

#include "std.h"
#include "mem.h"
#include "../deps/math/math.h"

/* Forward declarations to avoid circular dependencies */
struct Sprite;
struct Actor;
struct Coordinates;
struct ScreenCoordinates;
struct Transformation;
struct Shape;

/* CONSTANTS =============================================================== */

/* Collision types */
#define CL_TYPE_ACTOR 0x01
#define CL_TYPE_ENVIRONMENT 0x02
#define CL_TYPE_DEFAULT 0x04

#define SP_GRID_SIZE 63
#define SP_GRID_HALF (SP_GRID_SIZE / 2)
#define SP_GRID_VIS_SIZE 1         /* Camera viewport size (1 grid slot) */
#define SP_GRID_SCALE 100          /* World units per grid cell (100:1) */
#define SP_MAX_RENDER_ASSETS 256

/* TRANSFORMATIONS ========================================================= */

/* 2D - ORTHOGONAL CAMERA FOR NOW */
typedef struct Camera {
    struct Coordinates *position;
    struct Coordinates *prevPos;
    struct ScreenCoordinates *resolution;

    unsigned char gridMinX;
    unsigned char gridMinY;
    unsigned char gridMinZ;

    unsigned char gridMaxX;
    unsigned char gridMaxY;
    unsigned char gridMaxZ;
} Camera;

typedef struct Collision {
    void *collidable;
    unsigned char type;
} Collision;

/* Culling grid system ===================================================== */

extern struct List *sp_visgrid[SP_GRID_SIZE][SP_GRID_SIZE][SP_GRID_SIZE];
extern struct Actor *renderQueue[SP_MAX_RENDER_ASSETS];	
extern struct Camera *sp_globalCamera;
extern struct Camera *sp_cameras[SP_GRID_SIZE];

/* PROTOTYPES ============================================================== */

Coordinates *sp_createCoordinates(long x, long y, int z);
ScreenCoordinates *sp_createScreenCoordinates(unsigned int x, unsigned int y);
void sp_calculateTranslation(
    struct Transformation *transformation,
    long *totalOffsetX,
    long *totalOffsetY,
    unsigned long gametick
);
bool sp_addActorToVisGrid(struct Actor *actor);
bool sp_removeActorFromVisGrid(
    unsigned char vis_x,
    unsigned char vis_y,
    unsigned char vis_z,
    unsigned int index
);

Camera *sp_createCamera(
    Coordinates *position,
    ScreenCoordinates *resolution
);
void sp_setGlobalCamera(Camera *camera);
void sp_destroyCamera(Camera *camera);
void sp_initCameras(void);
void sp_checkCameras(void);

#define SP_WORLD_TO_SCREEN(worldX, worldY, camX, camY, resX, resY, outX, outY) \
    do { \
        (outX) = ((resX) >> 1) + ((worldX) - (camX)); \
        (outY) = ((resY) >> 1) + ((worldY) - (camY)); \
    } while (0)

bool sp_isInFrustrum(int screenX, int screenY, struct Sprite *sprite);
bool sp_isShapeInFrustrum(int screenX, int screenY, struct Shape *shape);
void sp_renderQueueApplyZOrdering(int length);

// Collision
void sp_addCollisions(struct Actor *actor, struct Actor *otherActor);
void sp_checkCollisions(struct Actor *actor);
void sp_clearCollisions(struct Actor *actor);
bool sp_isColliding(struct Actor *actor);

void sp_bounceBack(
    struct Actor *actor,
    int prevX,
    int prevY,
    int prevZ
);

/* Backward compatibility aliases */
#define sp_addAssetTosp_visgrid sp_addActorToVisGrid
#define sp_removeAssetFromsp_visgrid sp_removeActorFromVisGrid
#define sp_setsp_globalCamera sp_setGlobalCamera
#define sp_init_cameras sp_initCameras
#define sp_check_cameras sp_checkCameras

#endif
