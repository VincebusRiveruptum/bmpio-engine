#ifndef GAME_H
#define GAME_H

#include "ENGINE.H"

/* Cl stands for Collision*/
#define CL_TYPE_ACTOR 0x01
#define CL_TYPE_ENVIRONMENT 0x02
#define CL_TYPE_DEFAULT 0x04

#define MAX_COLLISIONS 8

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
	/*
		tyes could be:
			BEHAVIOUR
			EVENT
			CONDITION
	*/
	struct List *triggers;	// List of action triggers
	struct List *behaviour;	// List of action behaviour
	struct List *condition;	// List of action conditions
	struct List *events;	// List of action events
	struct List *logic;	// List of action logic
	struct List *animations;	// List of action animations
	struct List *sounds;	// List of action sounds
	struct List *visuals;	// List of action visuals
	struct List *collisions;	// List of action collisions
	struct List *collidables;	// List of action collidables
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
/* Actor data that is constant and loaded after the game starts*/
typedef struct Actor{
	char name[32];
	char description[256];
    
    struct Stats *stats;
	
	// Defined behaviour
	struct List *actionList;  // Array or list of possible actions
}Actor;

/* This cound be an actor instance that is currently playing */
typedef struct Asset{
	unsigned long id;
	struct Actor *actor;
	struct Coordinates *coordinates;
	struct Collision collisions[MAX_COLLISIONS];

	// Stuff for culling
	unsigned char vis_prevX;
	unsigned char vis_prevY;
	unsigned char vis_prevZ;

	unsigned char vis_currentX;
	unsigned char vis_currentY;
	unsigned char vis_currentZ;
	
}Asset;

/* PROTOYPES ===========================================================================*/

/* Possible actionns 
	idle
	walk
	attack
	defend
*/

Asset *gm_createAsset(Actor *actor, Coordinates *coordinates);
void gm_insertAsset(Asset *asset);
Asset *gm_getAssetByIndex(unsigned char vis_x, unsigned char vis_y, unsigned char vis_z);
void gm_destroyAsset(Asset *asset);

#endif
