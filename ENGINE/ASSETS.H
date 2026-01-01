/*
	This file handles bmp file operations
*/

#ifndef ENGINE_ASSETS_H
#define ENGINE_ASSETS_H

// STD IMPORTS
#include "STD.H"

/* PROTOYPES*/
Animation *createAnimation(Coordinates *coordinates);}
Sprite *createSprite();
void loadSprite(Sprite *sprite, char *fileName, Coordinates *coordinates, unsigned char maskColor);
void loadAnimationFrames(Animation *animation, char **frameArray);
void drawAnimations(unsigned long gametick);

/* BMP STUFF ===================================================================================*/

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
    struct Color *palette;          // Color palette object
    long width;
    long height;
}BMPdata;

/* END BMP STUFF ===========================================================================*/

/* SPRITE ===================================================================================*/

typedef struct Sprite{
	struct BMPdata *bmpData;
	struct Coordinates *coordinates;
	char maskColor;
}Sprite;

typedef struct Animation{
	struct List *frames;			// SPRITE LIST
	int length;
	int frameDelay;
	bool loop;

	struct Coordinates *coordinates;
	char maskColor;

	struct List *transformationList;		// An animation can have many transformations at the same time.
}Animation;

#endif