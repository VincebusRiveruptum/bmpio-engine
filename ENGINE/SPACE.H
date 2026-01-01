
#ifndef ENGINE_SPACE_H
#define ENGINE_SPACE_H

#include "STD.H"

/* TRANSFORMATIONS ===========================================================================*/
typedef struct RotationTransformation{
	int angle;		// angle step per tick
	int current;    // current angle
}RotationTransformation;

typedef struct TranslationTransformation{
	/*
		For now, we just use destination coordinates and loop. The origin will be the animation base coordinates.
	*/
	struct Coordinates *dest;
	bool loop;
}TranslationTransformation;

typedef struct Transformation {
	char *type;				// ROTATION, MOVEMENT, etc
	void *data;				// Data for the transformation
}Transformation;

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

// GRID SYSTEM
#define SP_GRID_SIZE 255

struct Asset;
struct Asset *visGrid[SP_GRID_SIZE][SP_GRID_SIZE];

/* PROTOYPES ===========================================================================*/

Coordinates *createCoordinates(long x, long y, int z);
ScreenCoordinates *createScreenCoordinates(unsigned int x, unsigned int y);
void calculateTranslation(Transformation *transformation, long *totalOffsetX, long *totalOffsetY, unsigned long gametick);

#endif
