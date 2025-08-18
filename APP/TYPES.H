#ifndef TYPES_H
#define TYPES_H

#define PI 3.14159

#define true 1
#define false 0

typedef unsigned char bool;
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

// ==================================================================================
// ASSET TYPES
// ==================================================================================

#define IMAGE 1
#define SOUND 2
#define TEXT 3

typedef struct Asset{
	unsigned long int id;
    char *name;
    char *path;
    unsigned int type;
    bool lazy;
    void *data;                 // Is this a good idea? Dunno
} Asset;


// ==================================================================================
//	AUDIT TYPES
// ==================================================================================

#define NO_LOGGING 0
#define FILE_LOGGING 1
#define CONSOLE_LOGGING 2
#define FILE_CONSOLE_LOGGING 3

// ======

#endif