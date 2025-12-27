#ifndef ENGINE_TYPES_H
#define ENGINE_TYPES_H

#include "STD.H"

typedef struct FileHeader{
	unsigned char id[2];
	long size;
	int res1[2];
	long offset;	
}FileHeader;

typedef struct InfoHeader{
	long hsize;
	long x;
	long y;
	int numColorPlanes;
	int bitsPerPixel;
	long compressionMethod;
	long imgSize;
	long resX;
	long resY;
	long numColors;
	long numImportantColors;
}InfoHeader;

typedef struct Color{
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char i;
}Color;

typedef struct BMPfile{
	struct FileHeader fh;
	struct InfoHeader ih;
	//unsigned char **bmp;
    struct BMPdata *bmpData;
}BMPfile;

typedef struct BMPdata{                     // Raw bmp data
    unsigned char **bmp;
    struct Color *palette;          // Color palette object
    long width;
    long height;
}BMPdata;

typedef struct Animation{
	struct List *frames;
	int length;
	int frameDelay;
	bool loop;

	struct Coordinates *coordinates;
	char maskColor;
}Animation;

typedef struct Sprite{
	struct BMPdata *bmpData;
	struct Coordinates *coordinates;
	char maskColor;
}Sprite;

// ==================================================================================
// ASSET TYPES
// ==================================================================================

#define IMAGE 1
#define SOUND 2
#define TEXT 3

typedef struct Asset{
	unsigned long int id;
    char *name;
    char *path;
    unsigned int type;
    bool lazy;
    void *data;                 // Is this a good idea? Dunno
} Asset;


// ==================================================================================
//	AUDIT TYPES
// ==================================================================================

#define NO_LOGGING 0
#define FILE_LOGGING 1
#define CONSOLE_LOGGING 2
#define FILE_CONSOLE_LOGGING 3



// ==================================================================================
// ACTOR
// ==================================================================================

/*
	Action triggeres could be 
		- External Eavents
		- User Input (keyboard, mouse, etc)
		- Time (timers, etc)
		- AI (AI, etc)

	Depending on the trigger and action type, the game executes an specific method which uses context data from the 
	environment, involved actors, user, time, etc.

	Example: Renamon is an actor that can be controleld by the user. if the action is 'jump', which is an input triggered by:
		- User Input (keyboard, mouse, etc)
		- AI (AI, etc)
	If the trigger was the USER, then depending on the actor's coordinates, the function for jumping is executed and the character 
	movement follows the user input (keyboard, mouse, etc).

	If the trigger was AI, then depending if the actor reacted due to collision from another actor, the function for jumping is executed and the character jumps over the other actor.

	Actor lifecycle:
		- Initialize
		- Load	
			Load Actor assets
				Sprites
				Sounds
				Actions
				Triggers
		- Update
			Check type
				User
					Check User's input
				AI
					Check AI's logic
						Check timers
						Check conditions
						Check environment
							Check collision
							Check distance
							Check visibility		
		- Render
			Render on screen visibility portal
			Hide if not visible
		- Free
			Free memory
			Free environment
			Free actions
			Free stats
			Free coordinates
			Free actor

	Instance Actor in Main Loop:
		
		Actor actor;
		
		createActor(&actor);
		
		main loop:	
			drawEnv();
			drawActors();
*/

// Forward declarations for pointers
struct List; 

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

typedef struct Action{
	char name[32];
	struct List *triggers;	// List of action triggers
	struct List *frames;	// List of frames BMP for now...
    
    void (*onTrigger)(struct Actor *self); // logic for this action
}Action;

#define ACTOR_FLAG_ACTIVE     0x01
#define ACTOR_FLAG_VISIBLE    0x02
#define ACTOR_FLAG_COLLIDABLE 0x04

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

#endif
	