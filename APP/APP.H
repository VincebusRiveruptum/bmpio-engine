/*
	This file handles bmp file operations
*/

#include "types.h"
#include <conio.h>
#include <dos.h>

// DEPENDENCES

#include "../DEPS/DATA/DATA.H"
#include "../DEPS/ENV/TYPES.H"

// PROTOTYPES

BMPfile *loadBMPfile(char *fileName);
void drawBitmap(BMPdata **bmpData, unsigned int x, unsigned int y, int maskcolor);
void addBMPtoList(List **bmpList, BMPdata *bmpData);
void drawList(List *list);
void setPalette(Color *palette);

// GLOBAL VARS

extern Config *gameConfig;
extern List *bmpList;
extern Color *globalPalette;
