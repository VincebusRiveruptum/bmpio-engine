#ifndef HAL_MODX_H
#define HAL_MODX_H

#include "std.h"

/* CONSTANTS =============================================================== */

#define PAGE_SIZE 16000
#define NUM_PAGES 3

#define VID_WIDTH 320
#define VID_HEIGHT 200

#define ENABLE_PAGE_FLIPPING 1

/* GLOBALS ================================================================= */

extern unsigned long pageOffsets[NUM_PAGES];
extern unsigned char currentPage;
extern unsigned char nextPage;

/* PROTOTYPES ============================================================== */

void hal_modx_init(void);
void hal_modx_flipPage(void);
void hal_modx_clearScreen(void);

void hal_modx_selectPlane(unsigned char plane);
void hal_modx_putPixel(int x, int y, char color);
unsigned char hal_modx_getPixel(int x, int y);

void hal_modx_drawRect(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    unsigned char color
);

void hal_modx_fastFillRect(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    unsigned char color
);

/* Backward compatibility aliases */
#define hal_vid_flipPage     hal_modx_flipPage
#define hal_vid_clearScreen  hal_modx_clearScreen
#define hal_vid_putPixelX    hal_modx_putPixel
#define hal_vid_getPixelX    hal_modx_getPixel
#define hal_vid_drawRect     hal_modx_drawRect
#define hal_vid_fastFillRect hal_modx_fastFillRect

#define v_putPixelX          hal_modx_putPixel
#define v_flipPage           hal_modx_flipPage
#define v_clearScreen        hal_modx_clearScreen
#define v_drawRect           hal_modx_drawRect
#define v_fastFillRect       hal_modx_fastFillRect

#endif
