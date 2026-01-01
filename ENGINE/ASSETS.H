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

/* PROTOYPES ===========================================================================*/

BMPfile *as_loadBMPfile(char *fileName);
Animation *as_createAnimation();
Sprite *as_createSprite();
bool as_loadSprite(Sprite *sprite, char *fileName, unsigned char maskColor);
void as_loadAnimationFrames(Animation *animation, char **frameArray);
void as_drawAnimations(unsigned long gametick);

void as_drawBitmap(BMPdata **bmpData, int x, int y, int maskcolor);
void as_drawBitmapPlaneBatch(BMPdata **bmpData, int x, int y, int maskcolor);
void as_drawBitmapTransform(BMPdata **bmpData, int x, int y, int maskcolor, int angle);

void as_drawSprites(unsigned long gametick);

void as_addAnimationToTable(Animation *animation); /* TODO: REFACTOR IN PROGRESS*/

#endif
