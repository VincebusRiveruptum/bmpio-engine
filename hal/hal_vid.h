
#ifndef HAL_VID_H
#define HAL_VID_H

#include "std.h"

#define HAL_VIDEO_BUFFER_SIZE 8192
#define HAL_SHOW_MSG 0
#define HAL_NO_MSG 1

/* CONSTANTS =============================================================== */

#define PAGE_SIZE 16000
#define NUM_PAGES 3

#define VID_WIDTH 320
#define VID_HEIGHT 200

#define ENABLE_PAGE_FLIPPING 1

enum VideoModeType {
    HAL_VID_TEXT,
    HAL_VID_GRAPH
};

enum VideoMode {
    // Text
    HAL_VID_80X25,
    HAL_VID_80X43,
    HAL_VID_80X50,
    HAL_VID_80X60,
    HAL_VID_132X25,
    HAL_VID_132X43,
    HAL_VID_132X50,
    HAL_VID_132X60,

    // Graphics
    HAL_VID_MODE13,
    HAL_VID_MODEX,
    HAL_VID_MODEY,
};

// Text-mode vars.
extern unsigned short *textmemptr;      /* Video buffer pointer */
extern unsigned short *backbuffer;    /*  Backup buffer */

extern unsigned char VIDEO_COLS;
extern unsigned char VIDEO_ROWS;

extern unsigned char currentCursorX;
extern unsigned char currentCursorY;

// Graphics mode vars.
extern int hal_vid_currentMode;

extern unsigned long pageOffsets[NUM_PAGES];
extern unsigned char currentPage;
extern unsigned char nextPage;

// Protoypes ========================================================

unsigned char hal_vid_setVideoMode(unsigned char mode, unsigned char show_msg);
void hal_vid_clearBuffer(unsigned short *buffer);

void hal_vid_cycleVideoModes();

void hal_vid_refresh();
void hal_vid_putCursor(unsigned char x, unsigned char y);


// Graphics modes ===================================================
//void hal_vid_getCurrentPage();
//void hal_vid_setCurrentPage(unsigned char page);
void hal_vid_flipPage();
void hal_vid_waitVsync();

unsigned short hal_vid_getVideoBufferSize();

void hal_vid_putPixelX(int x, int y, char color);
unsigned char hal_vid_getPixelX(int x, int y);

void hal_vid_setPal(
    char color,
    unsigned char r,
    unsigned char g,
    unsigned char b
);
void hal_vid_drawPalette();

void hal_vid_clearScreen();

void hal_vid_drawRect(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    unsigned char color
);

void hal_vid_fastFillRect(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    unsigned char color
);

// Deprecated
//void v_setTXTMode();


#endif
