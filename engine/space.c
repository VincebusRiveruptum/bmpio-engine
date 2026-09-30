#include "space.h"
#include "game.h"
#include "mem.h"

struct List *sp_visgrid[SP_GRID_SIZE][SP_GRID_SIZE][SP_GRID_SIZE];
struct Actor *renderQueue[SP_MAX_RENDER_ASSETS] = {NULL};
struct Camera *sp_globalCamera = NULL;
struct Camera *sp_cameras[SP_GRID_SIZE] = {NULL};

Coordinates *sp_createCoordinates(long x, long y, int z)
{
    Coordinates *newCoordinates = NULL;

    newCoordinates = (Coordinates *)mem_arena_alloc(
        sceneArena,
        sizeof(Coordinates)
    );
    if (newCoordinates) {
        newCoordinates->x = x;
        newCoordinates->y = y;
        newCoordinates->z = z;
    }
    return newCoordinates;
}

ScreenCoordinates *sp_createScreenCoordinates(unsigned int x, unsigned int y)
{
    ScreenCoordinates *newScreenCoordinates = NULL;

    newScreenCoordinates = (ScreenCoordinates *)mem_arena_alloc(
        sceneArena,
        sizeof(ScreenCoordinates)
    );
    if (newScreenCoordinates) {
        newScreenCoordinates->x = x;
        newScreenCoordinates->y = y;
    }
    return newScreenCoordinates;
}

/* Helper Functions ======================================================= */

void sp_calculateTranslation(
    Transformation *transformation,
    long *totalOffsetX,
    long *totalOffsetY,
    unsigned long gametick
) {
    TranslationTransformation *translation = NULL;

    if (!transformation) {
        logger("\nError: Translation transformation is NULL");
        return;
    }

    translation = (TranslationTransformation *)transformation->data;

    if (translation != NULL && translation->dest != NULL) {
        if (*totalOffsetX < translation->dest->x) {
            if (translation->loop == true) {
                *totalOffsetX = *totalOffsetX +
                    ((gametick | 1) % translation->dest->x);
            } else {
                (*totalOffsetX)++;
            }
        }

        if (*totalOffsetX > translation->dest->x) {
            if (translation->loop == true) {
                *totalOffsetX = *totalOffsetX -
                    ((gametick | 1) % translation->dest->x);
            } else {
                (*totalOffsetX)--;
            }
        }

        if (*totalOffsetY < translation->dest->y) {
            if (translation->loop == true) {
                *totalOffsetY = *totalOffsetY +
                    ((gametick | 1) % translation->dest->y);
            } else {
                (*totalOffsetY)++;
            }
        }

        if (*totalOffsetY > translation->dest->y) {
            if (translation->loop == true) {
                *totalOffsetY = *totalOffsetY -
                    ((gametick | 1) % translation->dest->y);
            } else {
                (*totalOffsetY)--;
            }
        }
    } else {
        logger("\nError: Translation data or dest is NULL");
    }
}

/* Culling ================================================================ */

bool sp_addAssetToVisGrid(struct Actor *asset)
{
    int gx = 0;
    int gy = 0;
    int gz = 0;

    if (!asset || !asset->coordinates) {
        logger("\nError: Actor or coordinates NULL");
        return false;
    }

    gx = (int)(asset->coordinates->x + SP_GRID_HALF);
    gy = (int)(asset->coordinates->y + SP_GRID_HALF);
    gz = (int)(asset->coordinates->z + SP_GRID_HALF);

    /* Normalize coordinates to grid size and check bounds */
    if (gx < 0 || gx >= SP_GRID_SIZE ||
        gy < 0 || gy >= SP_GRID_SIZE ||
        gz < 0 || gz >= SP_GRID_SIZE) {
        logger("\nError: Actor coordinates are out of bounds");
        return false;
    }

    addGenericNode(&sp_visgrid[gx][gy][gz], (void *)asset, sceneArena);
    return true;
}

bool sp_removeAssetFromVisGrid(
    unsigned char vis_x,
    unsigned char vis_y,
    unsigned char vis_z,
    unsigned int index
) {
    if (vis_x >= SP_GRID_SIZE ||
        vis_y >= SP_GRID_SIZE ||
        vis_z >= SP_GRID_SIZE) {
        logger("\nError: Index out of bounds");
        return false;
    }

    if (sp_visgrid[vis_x][vis_y][vis_z] == NULL) {
        logger("\nError: Actor list is NULL");
        return false;
    }

    deleteNodeByIndex(&sp_visgrid[vis_x][vis_y][vis_z], index);
    return true;
}

/* CAMERA ================================================================= */

void sp_calculateCameraBounds(Camera *camera)
{
    int camGridX = 0;
    int camGridY = 0;
    int camGridZ = 0;

    if (!camera || !camera->position) {
        return;
    }

    camGridX = (int)(camera->position->x / SP_GRID_SCALE) + SP_GRID_HALF;
    camGridY = (int)(camera->position->y / SP_GRID_SCALE) + SP_GRID_HALF;
    camGridZ = (int)(camera->position->z / SP_GRID_SCALE) + SP_GRID_HALF;

    camera->gridMinX = (camGridX - SP_GRID_VIS_SIZE < 0) ?
        0 : (unsigned char)(camGridX - SP_GRID_VIS_SIZE);
    camera->gridMinY = (camGridY - SP_GRID_VIS_SIZE < 0) ?
        0 : (unsigned char)(camGridY - SP_GRID_VIS_SIZE);
    camera->gridMinZ = (camGridZ - SP_GRID_VIS_SIZE < 0) ?
        0 : (unsigned char)(camGridZ - SP_GRID_VIS_SIZE);

    camera->gridMaxX = (camGridX + SP_GRID_VIS_SIZE >= SP_GRID_SIZE) ?
        (unsigned char)(SP_GRID_SIZE - 1) :
        (unsigned char)(camGridX + SP_GRID_VIS_SIZE);
    camera->gridMaxY = (camGridY + SP_GRID_VIS_SIZE >= SP_GRID_SIZE) ?
        (unsigned char)(SP_GRID_SIZE - 1) :
        (unsigned char)(camGridY + SP_GRID_VIS_SIZE);
    camera->gridMaxZ = (camGridZ + SP_GRID_VIS_SIZE >= SP_GRID_SIZE) ?
        (unsigned char)(SP_GRID_SIZE - 1) :
        (unsigned char)(camGridZ + SP_GRID_VIS_SIZE);
}

Camera *sp_createCamera(Coordinates *position, ScreenCoordinates *resolution)
{
    Camera *newCamera = NULL;

    if (!position || !resolution) {
        logger("[sp_createCamera]: Error, Position or resolution is NULL");
        return NULL;
    }

    newCamera = (Camera *)malloc(sizeof(Camera));
    if (!newCamera) {
        return NULL;
    }
    newCamera->position = position;
    newCamera->prevPos = NULL;
    newCamera->resolution = resolution;

    sp_calculateCameraBounds(newCamera);

    logger(
        "[sp_createCamera]: Camera bounds [%d-%d][%d-%d][%d-%d]",
        newCamera->gridMinX,
        newCamera->gridMaxX,
        newCamera->gridMinY,
        newCamera->gridMaxY,
        newCamera->gridMinZ,
        newCamera->gridMaxZ
    );
    return newCamera;
}

void sp_setGlobalCamera(Camera *camera)
{
    if (!camera) {
        logger("[sp_setGlobalCamera]: Error, Camera is NULL");
        return;
    }
    sp_globalCamera = camera;
}

void sp_destroyCamera(Camera *camera)
{
    if (!camera) {
        logger("[sp_destroyCamera]: Error, Camera is NULL");
        return;
    }
    free(camera);
}

/* Make render queue based on global camera */
void sp_initCameras(void)
{
    int i = 0;
    int j = 0;
    int k = 0;
    int qIndex = 0;
    List *assetList = NULL;
    Node *node = NULL;
    Actor *asset = NULL;

    if (!sp_globalCamera) {
        logger("[sp_initCameras]: Error, Global camera is NULL");
        return;
    }

    /* Clear render queue */
    memset(renderQueue, 0, sizeof(renderQueue));

    /* Generate render queue scanning visible region */
    for (i = sp_globalCamera->gridMinX; i <= sp_globalCamera->gridMaxX; i++) {
        for (j = sp_globalCamera->gridMinY;
             j <= sp_globalCamera->gridMaxY;
             j++) {
            for (k = sp_globalCamera->gridMinZ;
                 k <= sp_globalCamera->gridMaxZ;
                 k++) {
                if (sp_visgrid[i][j][k] == NULL) {
                    continue;
                }

                assetList = sp_visgrid[i][j][k];
                if (assetList == NULL) {
                    continue;
                }

                node = assetList->firstNode;
                while (node != NULL) {
                    asset = (Actor *)node->data;

                    asset->vis_prevX = asset->vis_currentX;
                    asset->vis_prevY = asset->vis_currentY;
                    asset->vis_prevZ = asset->vis_currentZ;

                    asset->vis_currentX = i;
                    asset->vis_currentY = j;
                    asset->vis_currentZ = k;

                    if (qIndex < SP_MAX_RENDER_ASSETS) {
                        renderQueue[qIndex] = asset;
                        qIndex++;
                    }
                    node = node->next;
                }
            }
        }
    }

    sp_renderQueueApplyZOrdering(qIndex);
}

void sp_checkCameras(void)
{
    if (!sp_globalCamera) {
        return;
    }

    if (sp_globalCamera->prevPos == NULL) {
        sp_globalCamera->prevPos = sp_createCoordinates(
            sp_globalCamera->position->x,
            sp_globalCamera->position->y,
            sp_globalCamera->position->z
        );
    }

    if (sp_globalCamera->position->x != sp_globalCamera->prevPos->x ||
        sp_globalCamera->position->y != sp_globalCamera->prevPos->y ||
        sp_globalCamera->position->z != sp_globalCamera->prevPos->z) {
        /* Recalculate bounds since camera moved */
        sp_calculateCameraBounds(sp_globalCamera);
        sp_initCameras();

        sp_globalCamera->prevPos->x = sp_globalCamera->position->x;
        sp_globalCamera->prevPos->y = sp_globalCamera->position->y;
        sp_globalCamera->prevPos->z = sp_globalCamera->position->z;
    }
}

bool sp_isInFrustrum(int screenX, int screenY, struct Sprite *sprite)
{
    int spriteWidth = 0;
    int spriteHeight = 0;

    if (!sprite || !sprite->bmpData) {
        logger("[sp_isInFrustrum]: Error, sprite or bmpData is NULL");
        return false;
    }

    spriteWidth = (int)sprite->bmpData->width;
    spriteHeight = (int)sprite->bmpData->height;

    /* Treat screenX/Y as CENTER */
    if (screenX + (spriteWidth / 2) < 0 ||
        screenX - (spriteWidth / 2) > VID_WIDTH ||
        screenY + (spriteHeight / 2) < 0 ||
        screenY - (spriteHeight / 2) > VID_HEIGHT) {
        return false;
    }

    return true;
}

bool sp_isShapeInFrustrum(int screenX, int screenY, struct Shape *shape)
{
    int boxWidth = 0;
    int boxHeight = 0;
    Box *box = NULL;

    if (!shape) {
        logger("[sp_isShapeInFrustrum]: Error, shape is NULL");
        return false;
    }

    if (shape->type == GM_SHAPE_TYPE_BOX) {
        box = (Box *)shape->shapeObject;
        if (!box) {
            return false;
        }
        boxWidth = (int)box->width;
        boxHeight = (int)box->height;
        /* Treat screenX/Y as CENTER */
        if (screenX + (boxWidth / 2) < 0 ||
            screenX - (boxWidth / 2) > VID_WIDTH ||
            screenY + (boxHeight / 2) < 0 ||
            screenY - (boxHeight / 2) > VID_HEIGHT) {
            return false;
        }
    }

    return true;
}

void sp_renderQueueApplyZOrdering(int length)
{
    int i = 0;
    int j = 0;
    Actor *temp = NULL;

    if (!renderQueue || length <= 0) {
        return;
    }

    /* Insertion sort algorithm limited to active assets */
    for (i = 0; i < length; i++) {
        if (renderQueue[i] == NULL) {
            continue;
        }
        j = i;
        while (j > 0 &&
               renderQueue[j] != NULL &&
               renderQueue[j - 1] != NULL &&
               renderQueue[j]->coordinates->z <
               renderQueue[j - 1]->coordinates->z) {
            temp = renderQueue[j];
            renderQueue[j] = renderQueue[j - 1];
            renderQueue[j - 1] = temp;
            j--;
        }
    }
}


// Collisions
void sp_addCollisions(Actor *asset, Actor *otherAsset)
{
    int i = 0;

    if (!asset || !otherAsset) {
        return;
    }

    for (i = 0; i < MAX_COLLISIONS; i++) {
        if (asset->collisions[i] == NULL) {
            asset->collisions[i] = otherAsset;
            return;
        }
    }
}

void sp_bounceBack(
    Actor *asset,
    int prevX,
    int prevY,
    int prevZ
) {
    if (!asset || !asset->coordinates) {
        return;
    }

    asset->coordinates->x = prevX;
    asset->coordinates->y = prevY;
    asset->coordinates->z = prevZ;
}

void sp_checkCollisions(Actor *asset)
{
    int i = 0;
    int j = 0;
    int k = 0;
    int gridX = 0;
    int gridY = 0;
    int gridZ = 0;
    List *list = NULL;
    Node *node = NULL;
    Actor *other = NULL;

    if (!asset || !asset->coordinates) {
        return;
    }

    gridX = (int)(asset->coordinates->x / SP_GRID_SCALE) + SP_GRID_HALF;
    gridY = (int)(asset->coordinates->y / SP_GRID_SCALE) + SP_GRID_HALF;
    gridZ = (int)(asset->coordinates->z / SP_GRID_SCALE) + SP_GRID_HALF;

    sp_clearCollisions(asset);

    for (i = gridX - 1; i <= gridX + 1; i++) {
        for (j = gridY - 1; j <= gridY + 1; j++) {
            for (k = gridZ - 1; k <= gridZ + 1; k++) {
                if (i < 0 || i >= SP_GRID_SIZE ||
                    j < 0 || j >= SP_GRID_SIZE ||
                    k < 0 || k >= SP_GRID_SIZE) {
                    continue;
                }

                list = sp_visgrid[i][j][k];
                if (!list) {
                    continue;
                }

                node = list->firstNode;
                while (node != NULL) {
                    other = (Actor *)node->data;
                    if (other && other != asset) {
                        if (abs((int)(asset->coordinates->x -
                                     other->coordinates->x)) < 50 &&
                            abs((int)(asset->coordinates->y -
                                     other->coordinates->y)) < 50 &&
                            abs((int)(asset->coordinates->z -
                                     other->coordinates->z)) < 50) {
                            sp_addCollisions(asset, other);
                        }
                    }
                    node = node->next;
                }
            }
        }
    }
}

void sp_clearCollisions(Actor *asset)
{
    int i = 0;

    if (!asset) {
        return;
    }

    for (i = 0; i < MAX_COLLISIONS; i++) {
        asset->collisions[i] = NULL;
    }
}

bool sp_isColliding(Actor *asset)
{
    if (!asset) {
        return false;
    }
    return (asset->collisions[0] != NULL);
}
