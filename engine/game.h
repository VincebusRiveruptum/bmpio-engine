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
#define GM_ACTION_ATTACK 0x05
#define GM_ACTION_JUMP 0x06
#define GM_ACTION_DIE 0x07

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


typedef struct Action {
    unsigned int index;
    char name[32];
    unsigned char type; 

    struct Animation *animation;

    void (*update)(struct Actor *self);
} Action;

typedef struct Asset {
    char name[32];
    char description[256];
    
    struct Stats *stats;
    struct Action *actions[GM_MAX_ACTIONS];
    struct Action *currentAction;
} Asset;

typedef struct Actor {
    unsigned long id;
    struct Asset *actor;
    struct Shape *shape;

    struct Coordinates *coordinates;
    struct Coordinates *pointingTo;

    struct Actor *collisions[MAX_COLLISIONS];

    /* Culling tracking */
    unsigned char vis_prevX;
    unsigned char vis_prevY;
    unsigned char vis_prevZ;

    unsigned char vis_currentX;
    unsigned char vis_currentY;
    unsigned char vis_currentZ;
} Actor;

typedef struct AssetList {
    struct List *assets;
} AssetList;

/* GLOBAL VARIABLES ======================================================= */

extern struct Actor *renderQueue[SP_MAX_RENDER_ASSETS];
extern Actor *gm_player;

#define player gm_player

/* PROTOTYPES ============================================================== */

Actor *gm_createAsset(
    Asset *actor,
    Shape *shape,
    Coordinates *coordinates
);
void gm_insertAsset(Actor *asset);
Actor *gm_getAssetByIndex(
    unsigned char vis_x,
    unsigned char vis_y,
    unsigned char vis_z,
    unsigned int index
);
void gm_destroyAsset(Actor *asset);

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
    void (*update)(struct Actor *self)
);
Asset *gm_createActor(
    char *name,
    char *description,
    Stats *stats,
    Action *actions[]
);

void gm_listenEvents(void);
void gm_kbdInput(void);

bool gm_setCurrentAction(Asset *actor, unsigned char actionType);
void gm_cameraMove(int x, int y, int z);


#endif
