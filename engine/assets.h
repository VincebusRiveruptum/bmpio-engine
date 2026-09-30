/*
	This file handles bmp file operations
*/

#ifndef ENGINE_ASSETS_H
#define ENGINE_ASSETS_H

/* STD IMPORTS */
#include "std.h"
#include "space.h"
#include "../hal/hal_vid.h"
#include "../deps/data/data.h"

/* CONSTANTS */

#define TR_ROTATION 1
#define TR_TRANSLATION 2
#define TR_SCALE 3

#define GM_ANIMATION_MAX_FRAMES 64
#define GM_ANIMATION_MAX_TRANSFORMATIONS 8

#define GM_MASK_COLOR 255

#ifndef GM_SHAPE_TYPE_BOX
#define GM_SHAPE_TYPE_BOX      0x01
#define GM_SHAPE_TYPE_SPHERE   0x02
#define GM_SHAPE_TYPE_CYLINDER 0x03
#endif

#if defined(__WATCOMC__) || defined(__MSDOS__) || defined(DOS)
#include "../platform/dos/video/modex.h"
#include "../platform/dos/video/vgaregs.h"
#endif

typedef enum AssetType {
    A_SPRITE,
    A_ANIMATION,
    A_BACKGROUND
} AssetType;

typedef struct Color {
    unsigned char b;
    unsigned char g;
    unsigned char r;
    unsigned char i;
} Color;

typedef struct FileHeader {
    unsigned char id[2];
    long size;
    int res1[2];
    long offset;	
} FileHeader;

typedef struct InfoHeader {
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
} InfoHeader;

typedef struct BMPfile {
    struct FileHeader fh;
    struct InfoHeader ih;
    struct BMPdata *bmpData;
} BMPfile;

typedef struct BMPdata {
    unsigned char **bmp;
    struct Color *palette;
    long width;
    long height;
} BMPdata;

typedef struct Box {
    unsigned int width;
    unsigned int height;
    unsigned int depth;
} Box;

typedef struct Shape {
    void *shapeObject;
    unsigned char color;
    unsigned char type;
    bool isVisible;
} Shape;

typedef struct Sprite {
    struct BMPdata *bmpData;
    char maskColor;
} Sprite;

typedef struct Animation {
    struct Sprite *frames[GM_ANIMATION_MAX_FRAMES];
    int length;
    int frameDelay;
    bool loop;
    char maskColor;
    struct Transformation *
        transformationList[GM_ANIMATION_MAX_TRANSFORMATIONS];
} Animation;

typedef struct RotationTransformation {
    int angle;
    int current;
} RotationTransformation;

typedef struct TranslationTransformation {
    struct Coordinates *dest;
    bool loop;
} TranslationTransformation;

typedef struct Transformation {
    unsigned char type;
    void *data;
} Transformation;

/* PROTOTYPES ============================================================== */

BMPfile *as_loadBMPfile(char *fileName);
Animation *as_createAnimation(void);
Sprite *as_createSprite(void);

bool as_loadSprite(
    Sprite *sprite,
    char *fileName,
    unsigned char maskColor
);
void as_loadAnimationFrames(
    Animation *animation,
    char **frameArray,
    unsigned char maskColor
);

void as_drawBitmap(
    BMPdata **bmpData,
    int x,
    int y,
    int maskcolor,
    bool hflip
);
void as_drawBitmapPlaneBatch(
    BMPdata **bmpData,
    int x,
    int y,
    int maskcolor
);
void as_drawBitmapTransform(
    BMPdata **bmpData,
    int x,
    int y,
    int maskcolor,
    int angle,
    bool hflip
);

bool as_addTransformation(
    Animation *animation,
    Transformation *transformation
);
bool as_addRotationTransformation(
    Animation *animation,
    RotationTransformation *transformation
);
bool as_removeTransformation(Animation *animation, int index);

void as_drawBox(Shape *boxShape, int x, int y);
RotationTransformation *as_createRotationTransformation(
    int angle,
    int current
);
Shape *as_createShape(
    void *shapeObject,
    unsigned char color,
    unsigned char type,
    bool isVisible
);
Box *as_createBox(
    unsigned int width,
    unsigned int height,
    unsigned int depth
);

#endif
