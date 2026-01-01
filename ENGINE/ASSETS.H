/*
	This file handles bmp file operations
*/

#ifndef ENGINE_ASSETS_H
#define ENGINE_ASSETS_H

// STD IMPORTS
#include "STD.H"
#include "SPACE.H"
#include "VIDEO.H"
#include "../DEPS/DATA/DATA.H"

/* TYPES ===================================================================================*/

typedef enum AssetType{
	A_SPRITE,
	A_ANIMATION,
	A_BACKGROUND,	
}AssetType;

typedef struct Color{
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char i;
}Color;

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

typedef struct BMPfile{
	struct FileHeader fh;
	struct InfoHeader ih;
    struct BMPdata *bmpData;
}BMPfile;

typedef struct BMPdata{                     // Raw bmp data
    unsigned char **bmp;
    struct Color *palette;         			// Color palette object
    long width;
    long height;
}BMPdata;

typedef struct Sprite{
	struct BMPdata *bmpData;
	char maskColor;
}Sprite;

typedef struct Animation{
	struct List *frames;					// SPRITE LIST
	int length;
	int frameDelay;
	bool loop;
	char maskColor;

	struct List *transformationList;		// An animation can have many transformations at the same time.
}Animation;

typedef struct Asset{						// Asset object
	void *asset;							// This could be a sprite, animation, background, etc
	unsigned char type;						// Asset type
	struct Coordinates *coordinates;		// Asset coordinates, relative to the environment space
}Asset;

/* PROTOYPES ===========================================================================*/

BMPfile *loadBMPfile(char *fileName);
Animation *createAnimation();
Sprite *createSprite();
bool loadSprite(Sprite *sprite, char *fileName, unsigned char maskColor);
void loadAnimationFrames(Animation *animation, char **frameArray);
void drawAnimations(unsigned long gametick);

void drawBitmap(BMPdata **bmpData, int x, int y, int maskcolor);
void drawBitmapPlaneBatch(BMPdata **bmpData, int x, int y, int maskcolor);
void drawBitmapTransform(BMPdata **bmpData, int x, int y, int maskcolor, int angle);

void drawSprites(unsigned long gametick);

void addAnimationToTable(Animation *animation); /* TODO: REFACTOR IN PROGRESS*/

#endif

/*
	Each one has its own coordinates. 

	Asset - Actor - Sprite/Animation

	An asset coordinates are relative to the environment space.
	An actor coordinates are relative to the asset position.
	A sprite/animation coordinates are relative to the actor position.

	The environment space is the world space.

*/