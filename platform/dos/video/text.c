#include "text.h"

/* GLOBALS ================================================================= */

unsigned short *textmemptr = NULL;
unsigned short *backbuffer = NULL;

unsigned char VIDEO_COLS = 80;
unsigned char VIDEO_ROWS = 25;

unsigned char currentCursorX = 0;
unsigned char currentCursorY = 0;

/* FUNCTIONS =============================================================== */

void hal_txt_init(void)
{
    hal_txt_set25Lines();
    textmemptr = (unsigned short *)0xB8000;
}

void hal_txt_close(void)
{
    hal_txt_set25Lines();
}

void hal_txt_set25Lines(void)
{
    VIDEO_ROWS = 25;
    VIDEO_COLS = 80;
    _set80x25_asm();
}

void hal_txt_set43Lines(void)
{
    VIDEO_ROWS = 43;
    VIDEO_COLS = 80;
    _set80x43_asm();
}

void hal_txt_set50Lines(void)
{
    VIDEO_ROWS = 50;
    VIDEO_COLS = 80;
    _set80x50_asm();
}

void hal_txt_set80x60(void)
{
    VIDEO_ROWS = 60;
    VIDEO_COLS = 80;
    _set80x60_asm();
}

void hal_txt_set132x25(void)
{
    VIDEO_ROWS = 25;
    VIDEO_COLS = 132;
    _set132x25_asm();
}

void hal_txt_set132x43(void)
{
    VIDEO_ROWS = 43;
    VIDEO_COLS = 132;
    _set132x43_asm();
}

void hal_txt_set132x50(void)
{
    VIDEO_ROWS = 50;
    VIDEO_COLS = 132;
    _set132x50_asm();
}

void hal_txt_set132x60(void)
{
    VIDEO_ROWS = 60;
    VIDEO_COLS = 132;
    _set132x60_asm();
}

unsigned short hal_txt_getBufferSize(void)
{
    return (unsigned short)(VIDEO_COLS * VIDEO_ROWS);
}

void hal_txt_clearBuffer(unsigned short *buffer)
{
    int i = 0;
    int size = 0;

    if (!buffer) {
        return;
    }

    size = (int)hal_txt_getBufferSize();
    while (i < size) {
        buffer[i] = ' ';
        i++;
    }
}

void hal_txt_refresh(void)
{
    /* DOS writing to textmemptr (0xB8000) is direct */
}

void hal_txt_putCursor(unsigned char x, unsigned char y)
{
    unsigned short temp = 0;

    currentCursorX = x;
    currentCursorY = y;
    temp = (unsigned short)(currentCursorY * VIDEO_COLS + currentCursorX);

    outPortb(0x3D4, 14);
    outPortb(0x3D5, (unsigned char)(temp >> 8));
    outPortb(0x3D4, 15);
    outPortb(0x3D5, (unsigned char)temp);
}
