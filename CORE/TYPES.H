#ifndef TYPES_H
#define TYPES_H

#include "data.h"
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

typedef struct Color{
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char i;
}Color;

typedef struct BMPfile{
	struct FileHeader fh;
	struct InfoHeader ih;
	//unsigned char **bmp;
    struct BMPdata *bmpData;
}BMPfile;

typedef struct BMPdata{                     // Raw bmp data
    unsigned char **bmp;
    struct Color *palette;          // Color palette object
    long width;
    long height;
}BMPdata;

extern List *bmpList = NULL;
extern Color *globalPalette = NULL;

#endif