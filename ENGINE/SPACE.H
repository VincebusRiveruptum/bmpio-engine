
#ifndef ENGINE_SPACE_H
#define ENGINE_SPACE_H

#include "STD.H"

/* Forward declarations to avoid circular dependencies */
struct Sprite;
struct Asset;
struct Transformation;

/* CONSTANTS ===========================================================================*/
#define SP_GRID_SIZE 63
#define SP_GRID_HALF (SP_GRID_SIZE/2)
#define SP_GRID_VIS_SIZE 1			// Camera viewport size (1 grid slot)
#define SP_GRID_SCALE 100			// World units per grid cell (100:1 scale)

/* TRANSFORMATIONS ===========================================================================*/

// SPACE COORDINATES
typedef struct Coordinates{
	long x;     // 16.16 Fixed Point
	long y;     // 16.16 Fixed Point
	int z;
}Coordinates;

// SCREEN COORDINATES
typedef struct ScreenCoordinates{
	unsigned int x;
	unsigned int y;
}ScreenCoordinates;

/* 2D - ORTHOGONAL CAMERA FOR NOW */
typedef struct Camera{
	struct Coordinates *position;
	struct Coordinates *prevPos;
	struct ScreenCoordinates *resolution;

	unsigned char gridMinX;
	unsigned char gridMinY;
	unsigned char gridMinZ;

	unsigned char gridMaxX;
	unsigned char gridMaxY;
	unsigned char gridMaxZ;
}Camera;

/* Culling grid system ==================================================================*/

extern struct Asset *visGrid[SP_GRID_SIZE][SP_GRID_SIZE][SP_GRID_SIZE];
extern struct Asset *renderQueue[SP_GRID_SIZE];		// Current render  queue
extern struct Camera *globalCamera;
extern struct Camera *cameras[SP_GRID_SIZE];

/* PROTOYPES ===========================================================================*/

Coordinates *sp_createCoordinates(long x, long y, int z);
ScreenCoordinates *sp_createScreenCoordinates(unsigned int x, unsigned int y);
void sp_calculateTranslation(struct Transformation *transformation, long *totalOffsetX, long *totalOffsetY, unsigned long gametick);
bool sp_addAssetToVisGrid(struct Asset *asset);
bool sp_removeAssetFromVisGrid(struct Asset *asset);

Camera *sp_createCamera(Coordinates *position, ScreenCoordinates *resolution);
void sp_setGlobalCamera(Camera *camera);
void sp_destroyCamera(Camera *camera);
void sp_initCameras();
void sp_checkCameras();
ScreenCoordinates *sp_worldToScreen(Coordinates *worldPos, Camera *camera);
bool sp_isInFrustrum(ScreenCoordinates *screenPos, struct Sprite *sprite);
void sp_renderQueueApplyZOrdering();

#endif
