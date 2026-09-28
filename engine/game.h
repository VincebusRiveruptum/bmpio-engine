#ifndef GAME_H
#define GAME_H

#include "engine.h"

/* Collision types */
#define CL_TYPE_ACTOR 0x01
#define CL_TYPE_ENVIRONMENT 0x02
#define CL_TYPE_DEFAULT 0x04

/* ACTION TYPES */
#define GM_ACTION_DEFAULT 0x01
#define GM_ACTION_IDLE 0x02
#define GM_ACTION_WALK 0x03
#define GM_ACTION_RUN 0x04
#define GM_ACTION_SPRINT 0x05
#define GM_ACTION_ATTACK 0x06
#define GM_ACTION_JUMP 0x07
#define GM_ACTION_DIE 0x08
#define GM_ACTION_CROUCH 0x09

#define MAX_COLLISIONS 8
#define GM_MAX_ACTIONS 16

#define GM_SHAPE_TYPE_BOX 0x01
#define GM_SHAPE_TYPE_SPHERE 0x02
#define GM_SHAPE_TYPE_CYLINDER 0x03

/* TYPES =================================================================== */

typedef struct Stats {
    int health;
    int maxHealth;
    int attack;
    int defense;
    int speed;
    int intelligence;
    int agility;
    int luck;
    int experience;
    int level;
} Stats;

typedef struct Collision {
    void *collidable;
    unsigned char type;
} Collision;

typedef struct Action {
    unsigned int index;
    char name[32];
    unsigned char type; 

    struct Animation *animation;

    void (*update)(struct Asset *self);
} Action;

typedef struct Actor {
    char name[32];
    char description[256];
    
    struct Stats *stats;
    struct Action *actions[GM_MAX_ACTIONS];
    struct Action *currentAction;
} Actor;

typedef struct Asset {
    unsigned long id;
    struct Actor *actor;
    struct Shape *shape;

    struct Coordinates *coordinates;
    struct Coordinates *pointingTo;

    struct Asset *collisions[MAX_COLLISIONS];

    /* Culling tracking */
    unsigned char vis_prevX;
    unsigned char vis_prevY;
    unsigned char vis_prevZ;

    unsigned char vis_currentX;
    unsigned char vis_currentY;
    unsigned char vis_currentZ;
} Asset;

typedef struct AssetList {
    struct List *assets;
} AssetList;

/* GLOBAL VARIABLES ======================================================= */

extern struct Asset *renderQueue[SP_MAX_RENDER_ASSETS];
extern Asset *gm_player;

#define player gm_player

/* PROTOTYPES ============================================================== */

Asset *gm_createAsset(
    Actor *actor,
    Shape *shape,
    Coordinates *coordinates
);
void gm_insertAsset(Asset *asset);
Asset *gm_getAssetByIndex(
    unsigned char vis_x,
    unsigned char vis_y,
    unsigned char vis_z,
    unsigned int index
);
void gm_destroyAsset(Asset *asset);

/* ACTOR METHODS =========================================================== */

Stats *gm_createStats(
    int health,
    int maxHealth,
    int attack,
    int defense,
    int speed
);
Action *gm_createAction(
    char *name,
    unsigned char type,
    Animation *animation,
    void (*update)(struct Asset *self)
);
Actor *gm_createActor(
    char *name,
    char *description,
    Stats *stats,
    Action *actions[]
);

void gm_listenEvents(void);
void gm_kbdInput(void);

bool gm_setCurrentAction(Actor *actor, unsigned char actionType);
void gm_addCollisions(Asset *asset, Asset *otherAsset);
void gm_checkCollisions(Asset *asset);
void gm_clearCollisions(Asset *asset);
bool gm_isColliding(Asset *asset);

#endif
