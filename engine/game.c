#include "game.h"
#include "mem.h"

Actor *gm_player = NULL;
static unsigned long _gm_assetIdCounter = 1;

Actor *gm_createAsset(Asset *actor, Shape *shape, Coordinates *coordinates){
    Actor *newAsset = NULL;
    Coordinates *pointingTo = NULL;

    newAsset = (Actor *)mem_arena_alloc(sceneArena, sizeof(Actor));

    if (!newAsset) {
        return NULL;
    }

    memset(newAsset, 0, sizeof(Actor));

    pointingTo = (Coordinates *)mem_arena_alloc(
        sceneArena,
        sizeof(Coordinates)
    );
    if (!pointingTo) {
        return NULL;
    }
    pointingTo->x = 0;
    pointingTo->y = 0;
    pointingTo->z = 0;

    newAsset->id = _gm_assetIdCounter++;
    newAsset->actor = actor;
    newAsset->shape = shape;
    newAsset->coordinates = coordinates;
    newAsset->pointingTo = pointingTo;

    return newAsset;
}

void gm_insertAsset(Actor *asset)
{
    int vis_x = 0;
    int vis_y = 0;
    int vis_z = 0;

    if (!asset || !asset->coordinates) {
        logger("[gm_insertAsset]: Error, asset or coordinates NULL");
        return;
    }

    vis_x = (int)(asset->coordinates->x / SP_GRID_SCALE) + SP_GRID_HALF;
    vis_y = (int)(asset->coordinates->y / SP_GRID_SCALE) + SP_GRID_HALF;
    vis_z = (int)(asset->coordinates->z / SP_GRID_SCALE) + SP_GRID_HALF;

    if (vis_x < 0 || vis_x >= SP_GRID_SIZE ||
        vis_y < 0 || vis_y >= SP_GRID_SIZE ||
        vis_z < 0 || vis_z >= SP_GRID_SIZE) {
        logger("[gm_insertAsset]: Error, asset out of bounds");
        return;
    }

    asset->vis_currentX = (unsigned char)vis_x;
    asset->vis_currentY = (unsigned char)vis_y;
    asset->vis_currentZ = (unsigned char)vis_z;

    asset->vis_prevX = (unsigned char)vis_x;
    asset->vis_prevY = (unsigned char)vis_y;
    asset->vis_prevZ = (unsigned char)vis_z;

    logger(
        "[gm_insertAsset]: Inserting asset at %d, %d, %d",
        vis_x,
        vis_y,
        vis_z
    );
    sp_addAssetToVisGrid(asset);
}

Actor *gm_getAssetByIndex(
    unsigned char vis_x,
    unsigned char vis_y,
    unsigned char vis_z,
    unsigned int index
) {
    Node *node = NULL;

    if (sp_visgrid[vis_x][vis_y][vis_z] == NULL) {
        return NULL;
    }

    node = dat_getNodeByIndex(&sp_visgrid[vis_x][vis_y][vis_z], index);
    if (!node) {
        return NULL;
    }

    return (Actor *)node->data;
}

void gm_destroyAsset(Actor *asset)
{
    if (!asset) {
        return;
    }
    /* Assets are released when sceneArena is reset */
}

/* ACTOR METHODS =========================================================== */

Stats *gm_createStats(
    int health,
    int maxHealth,
    int attack,
    int defense,
    int speed
) {
    Stats *newStats = NULL;

    newStats = (Stats *)mem_arena_alloc(
        gameSessionArena,
        sizeof(Stats)
    );
    if (!newStats) {
        return NULL;
    }

    newStats->health = health;
    newStats->maxHealth = maxHealth;
    newStats->attack = attack;
    newStats->defense = defense;
    newStats->speed = speed;
    newStats->intelligence = 10;
    newStats->agility = 10;
    newStats->luck = 10;
    newStats->experience = 0;
    newStats->level = 1;

    return newStats;
}

Action *gm_createAction(
    char *name,
    unsigned char type,
    Animation *animation,
    void (*update)(struct Actor *self)
) {
    Action *newAction = NULL;

    newAction = (Action *)mem_arena_alloc(
        gameSessionArena,
        sizeof(Action)
    );
    if (!newAction) {
        return NULL;
    }

    if (name) {
        strncpy(newAction->name, name, 31);
        newAction->name[31] = '\0';
    } else {
        newAction->name[0] = '\0';
    }

    newAction->type = type;
    newAction->animation = animation;
    newAction->update = update;

    return newAction;
}

Asset *gm_createActor(
    char *name,
    char *description,
    Stats *stats,
    Action *actions[]
) {
    int i = 0;
    Asset *newActor = NULL;
    Action *genericAction = NULL;

    newActor = (Asset *)mem_arena_alloc(
        gameSessionArena,
        sizeof(Asset)
    );
    if (!newActor) {
        return NULL;
    }

    if (name) {
        strncpy(newActor->name, name, 31);
        newActor->name[31] = '\0';
    } else {
        newActor->name[0] = '\0';
    }

    if (description) {
        strncpy(newActor->description, description, 255);
        newActor->description[255] = '\0';
    } else {
        newActor->description[0] = '\0';
    }

    newActor->stats = stats;

    for (i = 0; i < GM_MAX_ACTIONS; i++) {
        newActor->actions[i] = NULL;
    }

    if (actions) {
        for (i = 0; i < GM_MAX_ACTIONS && actions[i] != NULL; i++) {
            newActor->actions[i] = actions[i];
        }
    }

    if (newActor->actions[0] == NULL) {
        logger("[gm_createActor]: Generic action assigned");
        genericAction = gm_createAction(
            "Generic",
            GM_ACTION_DEFAULT,
            NULL,
            NULL
        );
        newActor->actions[0] = genericAction;
    }

    newActor->currentAction = newActor->actions[0];
    return newActor;
}

bool gm_setCurrentAction(Asset *actor, unsigned char actionType)
{
    int i = 0;

    if (!actor) {
        return false;
    }

    for (i = 0; i < GM_MAX_ACTIONS; i++) {
        if (actor->actions[i] != NULL &&
            actor->actions[i]->type == actionType) {
            actor->currentAction = actor->actions[i];
            return true;
 
        }
    }

    logger(
        "[gm_setCurrentAction]: Action type %d not found for %s",
        actionType,
        actor->name
    );
    return false;
}


void gm_mainPlayerWalk(Actor *self)
{
    if (!self || !self->coordinates) {
        return;
    }
    self->coordinates->x += 2;
}

void gm_mainPlayerJump(Actor *self)
{
    if (!self || !self->coordinates) {
        return;
    }
    self->coordinates->z += 5;
}

void gm_mainPlayerIdle(Actor *self)
{
    if (!self) {
        return;
    }
    /* Idle tick update logic */
}
/*
void gm_cameraMove(Actor *self)
{
    if (!self || !sp_globalCamera || !sp_globalCamera->position) {
        return;
    }
    sp_globalCamera->position->x = self->coordinates->x;
    sp_globalCamera->position->y = self->coordinates->y;
    sp_globalCamera->position->z = self->coordinates->z;
}
*/

void gm_kbdInput(void)
{
    int prevX = 0;
    int prevY = 0;
    int prevZ = 0;

    if (!gm_player || !gm_player->coordinates) {
        return;
    }

    prevX = (int)gm_player->coordinates->x;
    prevY = (int)gm_player->coordinates->y;
    prevZ = (int)gm_player->coordinates->z;


	// Camera movement (LSHIFT + WASD)
    if(hal_inp_isKeyDown(HAL_KEY_LSHIFT) == true){
        if(hal_inp_isKeyDown(HAL_KEY_A) == true) gm_cameraMove(-16, 0, 0);
        if(hal_inp_isKeyDown(HAL_KEY_D) == true) gm_cameraMove(16, 0, 0);
        if(hal_inp_isKeyDown(HAL_KEY_W) == true) gm_cameraMove(0, -16, 0);
        if(hal_inp_isKeyDown(HAL_KEY_S) == true) gm_cameraMove(0, 16, 0);
        return; // Don't move player if camera is moving
    }

	if (hal_inp_isKeyDown(HAL_KEY_UP) || hal_inp_isKeyDown(HAL_KEY_W)) {
        gm_player->coordinates->y -= 4;
    }
    if (hal_inp_isKeyDown(HAL_KEY_DOWN) || hal_inp_isKeyDown(HAL_KEY_S)) {
        gm_player->coordinates->y += 4;
    }
    if (hal_inp_isKeyDown(HAL_KEY_LEFT) || hal_inp_isKeyDown(HAL_KEY_A)) {
        gm_player->coordinates->x -= 4;
    }
    if (hal_inp_isKeyDown(HAL_KEY_RIGHT) || hal_inp_isKeyDown(HAL_KEY_D)) {
        gm_player->coordinates->x += 4;
    }

    if (hal_inp_isKeyDown(HAL_KEY_SPACE)) {
        gm_setCurrentAction(gm_player->actor, GM_ACTION_JUMP);
        gm_player->coordinates->z += 4;
    } else {
        gm_setCurrentAction(gm_player->actor, GM_ACTION_IDLE);
    }

    sp_checkCollisions(gm_player);
	
    if (sp_isColliding(gm_player)) {
        sp_bounceBack(gm_player, prevX, prevY, prevZ);
    }

	// this uses the commented function
	// im still investigating what it was doing
    //gm_cameraMove(gm_player);
}

void gm_listenEvents(void)
{
    gm_kbdInput();
}

void gm_cameraMove(int x, int y, int z){

	sp_globalCamera->prevPos->x = sp_globalCamera->position->x;
	sp_globalCamera->prevPos->y = sp_globalCamera->position->y;
	sp_globalCamera->prevPos->z = sp_globalCamera->position->z;

	sp_globalCamera->position->x += x;
	sp_globalCamera->position->y += y;
	sp_globalCamera->position->z += z;
}