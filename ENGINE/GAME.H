#ifndef GAME_H
#define GAME_H

#include "ENGINE.H"

/* Cl stands for Collision*/
#define CL_TYPE_ACTOR 0x01
#define CL_TYPE_ENVIRONMENT 0x02
#define CL_TYPE_DEFAULT 0x04

/* ACTION TYPES*/
#define GM_ACTION_DEFAULT 0x01	// GENERIC TEST ACTION
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

/* TYPES ===================================================================================*/

typedef struct Stats{
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
}Stats;

typedef struct Collision{
	void *collidable;
	unsigned char type;
}Collision;

typedef struct Action{
	unsigned int index;
	char name[32];
	unsigned char type; 

	struct Animation *animation;

	void (*update)(struct Asset *self);
}Action;

/*
	I'm thinking on how i can determine an Actors animation frames bettween actions
	Like, if an actor is idle, it should load the idle sprites or assets involved 
	
	But, at some point an action is an action. An actor sohuld react by Behavour? Events? Condsitions?

	Also, when an actor can be playeble by the user?

	I have many questions.

	Check colissions sohuld nbe on the main loop, this meythod detects if an actor object collides with another actor object.
	To each actor involved, returns a list of actors that co;lided with it.

	COLLISION IDEA:

		- implement checkCollision() and call it in main loop
		= ActorPlaying has a collision array and if checkCollision find one between actors or objects it will
		send the collision to the actor's action system and store it in collisions array
	
	IF a actor is playable it must have :
		- Is playable flag enabled
		- Have an input map/system
		- In single player mode, the playable actor will be stored in a single global variable

*/

/*
	Actions describe what is happening.
	Logic decides when to change actions.
	Rendering only cares about the current action.
*/

/* Actor data that is constant and loaded after the game starts*/
typedef struct Actor{
	char name[32];
	char description[256];
    
    struct Stats *stats;
	struct Action *actions[GM_MAX_ACTIONS];  // Array or list of possible actions
	struct Action *currentAction;
}Actor;

/* This cound be an actor instance that is currently playing */
typedef struct Asset{
	unsigned long id;
	struct Actor *actor;				// Just for rendering
	struct Shape *shape;				// Could be used for collision or rendering as can be invisible

	struct Coordinates *coordinates;
	struct Coordinates *pointingTo;

	struct Collision collisions[MAX_COLLISIONS];

	// Stuff for culling
	unsigned char vis_prevX;
	unsigned char vis_prevY;
	unsigned char vis_prevZ;

	unsigned char vis_currentX;
	unsigned char vis_currentY;
	unsigned char vis_currentZ;
}Asset;

typedef struct AssetList{
	struct List *assets;
}AssetList;

/* 	GLOBAL VARIABLES ===========================================================================*/
extern Asset *player;

/* PROTOYPES ===========================================================================*/

/* Possible actionns 
	idle
	walk
	attack
	defend
*/

Asset *gm_createAsset(Actor *actor, Coordinates *coordinates);
void gm_insertAsset(Asset *asset);
Asset *gm_getAssetByIndex(unsigned char vis_x, unsigned char vis_y, unsigned char vis_z, unsigned int index);
void gm_destroyAsset(Asset *asset);

/* ACTOR METHODS ===========================================================================*/
Stats *gm_createStats(int health, int maxHealth, int attack, int defense, int speed);
Action *gm_createAction(char *name, unsigned char type, Animation *animation, void (*update)(struct Asset *self));
Actor *gm_createActor(char *name, char *description, Stats *stats, Action *actions[]);

void gm_listenEvents();
void gm_kbdInput();

bool gm_setCurrentAction(Actor *actor, unsigned char actionType);

#endif
