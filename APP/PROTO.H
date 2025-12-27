#ifndef PROTO_H
#define PROTO_H

#include "VARS.H"

bool checkConfig();
BMPfile *loadBMPfile(char *fileName);
void drawBitmap(BMPdata **bmpData, unsigned int x, unsigned int y, int maskcolor);
void drawBitmapDistorted(BMPdata **bmpData, unsigned int x, unsigned int y, int maskcolor, int angle);
void addBMPtoList(List **bmpList, BMPdata *bmpData);
void drawList(List *list);
void setPalette(Color *palette);

#endif