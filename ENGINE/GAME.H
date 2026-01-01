#ifndef GAME_H
#define GAME_H

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

typedef struct Action{
	char name[32];
	struct List *triggers;	// List of action triggers
	struct List *frames;	// List of frames BMP for now...
    
    void (*onTrigger)(struct Actor *self); // logic for this action
}Action;

typedef struct Actor{
    char id[32];
    char name[32];
    
    struct Action *actions;  // Array or list of possible actions
    struct Stats *stats;
	struct Coordinates *coordinates;

    // State
    struct Action *currentAction;
    int frameIndex;
    unsigned int flags;
}Actor;

/* PROTOYPES ===========================================================================*/

#endif
