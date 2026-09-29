#include "video.h"
#include "vgaregs.h"

#include "video.h"

unsigned short *textmemptr = NULL;
unsigned short *backbuffer = NULL;
int hal_vid_currentMode = HAL_VID_80X25;

// Graphics global vars
unsigned char currentPage = 0;
unsigned char nextPage = 1;

unsigned long pageOffsets[NUM_PAGES];


void hal_vid_set25Lines(void){
    VIDEO_ROWS = 25;
    VIDEO_COLS = 80;
    _set80x25_asm();
}

void hal_vid_set43Lines(void){
    VIDEO_ROWS = 43;
    VIDEO_COLS = 80;
    _set80x43_asm();
}

void hal_vid_set50Lines(void){
    VIDEO_ROWS = 50;
    VIDEO_COLS = 80;
    _set80x50_asm();
}

void hal_vid_set80x60(void){
    VIDEO_ROWS = 60;
    VIDEO_COLS = 80;
    _set80x60_asm();
}

void hal_vid_set132x25(void){
    VIDEO_ROWS = 25;
    VIDEO_COLS = 132;
    _set132x25_asm();
}

void hal_vid_set132x43(void){
    VIDEO_ROWS = 43;
    VIDEO_COLS = 132;
    _set132x43_asm();
}

void hal_vid_set132x50(void){
    VIDEO_ROWS = 50;
    VIDEO_COLS = 132;
    _set132x50_asm();
}

void hal_vid_set132x60(void){
    VIDEO_ROWS = 60;
    VIDEO_COLS = 132;
    _set132x60_asm();
}

unsigned char hal_vid_setVideoMode(VideoMode mode){
    switch(mode){
        case HAL_VID_80X25:
            hal_vid_set25Lines();
            break;
        case HAL_VID_80X43:
            hal_vid_set43Lines();
            break;
        case HAL_VID_80X50:
            hal_vid_set50Lines();
            break;
        case HAL_VID_80X60:
            hal_vid_set80x60();
            break;
        case HAL_VID_132X25:
            hal_vid_set132x25();
            break;
        case HAL_VID_132X43:
            hal_vid_set132x43();
            break;
        case HAL_VID_132X50:
            hal_vid_set132x50();
            break;
        case HAL_VID_132X60:
            hal_vid_set132x60();
            break;
        case HAL_VID_MODE13:
        case HAL_VID_MODEX:
        case HAL_VID_MODEY:
            _set200pxMode();
            break;
    }

    return mode;
}
unsigned short hal_vid_getVideoBufferSize(void){
    return VIDEO_COLS * VIDEO_ROWS;
}

void hal_vid_clearBuffer(unsigned short *buffer){
    int i=0;
    while(i < hal_vid_getVideoBufferSize()){
        buffer[i] = ' ';
        i++;
    }
}

void hal_vid_refresh(void){
    /* DOS writing to textmemptr is direct, no-op needed */
}

void hal_vid_putCursor(unsigned char x, unsigned char y){
    unsigned short temp;

    currentCursorX = x;
    currentCursorY = y;
    temp = currentCursorY * VIDEO_COLS + currentCursorX;

    outPortb(0x3D4, 14);
    outPortb(0x3D5, temp >> 8);
    outPortb(0x3D4, 15);
    outPortb(0x3D5, temp);
}

// Graphics Modes =========================================

void _set200pxMode(){
    int i = 0;

    _setVideoMode13();

    outPortw(CRTC_ADDR, 0x0011);
    outPortw(SEQU_ADDR, 0x0604);
    outPortw(CRTC_ADDR, 0xE317);
    outPortw(CRTC_ADDR, 0x0014);
    outPortw(SEQU_ADDR, 0x0F02);

    for (i = 0; i < NUM_PAGES; i++) {
        pageOffsets[i] = (unsigned long)i * PAGE_SIZE;
    }

    hal_vid_clearScreen();
}

/* Page buffering functions */
static void _setPage(unsigned char page){
    unsigned short start_addr = 0;

    start_addr = (unsigned short)pageOffsets[page];

    outPortw(CRTC_ADDR, (unsigned short)(0x0C | (start_addr & 0xFF00)));
    outPortw(
        CRTC_ADDR,
        (unsigned short)(0x0D | ((start_addr << 8) & 0xFF00))
    );
}

void hal_vid_flipPage(){
    hal_vid_waitVsync();
    _setPage(nextPage);
    currentPage = nextPage;
    nextPage = (currentPage + 1) % NUM_PAGES;
}

void hal_vid_setPal(
    char color,
    unsigned char r,
    unsigned char g,
    unsigned char b
) {
    outPortb(0x3c8, (unsigned char)color);
    outPortb(0x3c9, r);
    outPortb(0x3c9, g);
    outPortb(0x3c9, b);
}

/* Basic pixel plotting */
void hal_vid_putPixelX(int x, int y, char color){
    unsigned long offs = 0;

    outPortb(SEQU_ADDR, 0x02);
    outPortb(SEQU_ADDR + 1, (unsigned char)(0x01 << (x & 3)));

    if (ENABLE_PAGE_FLIPPING == 1) {
        offs = (unsigned long)((y << 6) + (y << 4) + (x >> 2)) +
            pageOffsets[nextPage];
    } else {
        offs = (unsigned long)((y << 6) + (y << 4) + (x >> 2));
    }

    v_putPixelASM(offs, (unsigned char)color);
}

void hal_vid_drawRect(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    unsigned char color
) {
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned long offs = 0;

    for (j = y1; j < y2; j++) {
        for (i = x1; i < x2; i++) {
            if (ENABLE_PAGE_FLIPPING == 1) {
                offs = (unsigned long)((j << 6) + (j << 4) + (i >> 2)) +
                    pageOffsets[nextPage];
            } else {
                offs = (unsigned long)((j << 6) + (j << 4) + (i >> 2));
            }

            outPortb(SEQU_ADDR, 0x02);
            outPortb(SEQU_ADDR + 1, 0x0F);
            v_putPixelASM(offs, color);
        }
    }
}

void hal_vid_fastFillRect(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    unsigned char color
) {
    unsigned int y = 0;
    unsigned int width_pixels = 0;
    unsigned int width_bytes = 0;
    unsigned int start_x_byte = 0;
    unsigned long page_offs = 0;
    unsigned long row_offs = 0;

    width_pixels = x2 - x1;
    width_bytes = width_pixels >> 2;
    start_x_byte = x1 >> 2;
    page_offs = pageOffsets[nextPage];

    outPortb(SEQU_ADDR, 0x02);
    outPortb(SEQU_ADDR + 1, 0x0F);

    for (y = y1; y < y2; y++) {
        row_offs = page_offs + (y << 6) + (y << 4) + start_x_byte;
        v_memsetVGAASM(row_offs, color, width_bytes);
    }
}
