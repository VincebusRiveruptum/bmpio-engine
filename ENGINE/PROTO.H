#ifndef ENGINE_PROTO_H
#define ENGINE_PROTO_H

#include "VARS.H"


bool checkConfig();
BMPfile *loadBMPfile(char *fileName);
void drawBitmap(BMPdata **bmpData, int x, int y, int maskcolor);
void drawBitmapPlaneBatch(BMPdata **bmpData, int x, int y, int maskcolor);
void drawBitmapDistorted(BMPdata **bmpData, int x, int y, int maskcolor, int angle);
void addBMPtoList(List **bmpList, BMPdata *bmpData);
void drawAnimations(unsigned long gametick);
void drawSprites(unsigned long gametick);
void setPalette(Color *palette);

Animation *createAnimation();
void loadAnimationFrames(Animation *animation, char **frameArray);
Sprite *createSprite();
bool loadSprite(Sprite *sprite, char *fileName, Coordinates *coordinates, unsigned char maskColor);
void render2d(unsigned long gametick);
void addAnimationToTable(Animation *animation);
void addGenericNode(List **list, void *data);

// TODO: MOVE TO ITS OWN MATH.C AND MATH.H MODULE
Coordinates *createCoordinates(long x, long y, int z);
ScreenCoordinates *createScreenCoordinates(unsigned int x, unsigned int y);


#endif