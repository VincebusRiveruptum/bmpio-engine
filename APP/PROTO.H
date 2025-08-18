#ifndef PROTO_H
#define PROTO_H

#pragma once

#include "../DEPS/DATA/TYPES.H"
#include "../DEPS/ENV/TYPES.H"

#include "TYPES.H"

bool checkConfig();
BMPfile *loadBMPfile(char *fileName);
void drawBitmap(BMPdata **bmpData, unsigned int x, unsigned int y, int maskcolor);
void addBMPtoList(List **bmpList, BMPdata *bmpData);
void drawList(List *list);
void setPalette(Color *palette);

#endif