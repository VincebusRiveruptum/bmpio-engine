/*
	This file handles bmp file operations
*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

#include "types.h"
#include "data.h"
#include "video.h"


#define PI 3.14159

BMPfile *loadBMPfile(char *fileName);
void drawBitmap(BMPdata **bmpData, unsigned int x, unsigned int y, int maskcolor);
void addBMPtoList(List **bmpList, BMPdata *bmpData);
void drawList(List *list);
void setPalette(Color *palette);