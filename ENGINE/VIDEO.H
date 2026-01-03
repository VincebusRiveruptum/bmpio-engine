#ifndef ENGINE_VIDEO_H
#define ENGINE_VIDEO_H

#include "STD.H"
#include "../HAL/VGAREGS.H"

/* CONSTANTS ===========================================================================*/

#define PAGE_SIZE 16000
#define NUM_PAGES 3

#define VID_WIDTH 320
#define VID_HEIGHT 200

/* PROTOYPES ===========================================================================*/

void v_clearScreen();
void v_setVideoMode13();
void v_set200pxMode(void);
void v_setTXTMode(void);
void v_putPixelX(int, int, char);
void v_putPixelASM(unsigned long offset, unsigned char color);
unsigned char v_getPixelX(int, int);
void v_setPal(char color, unsigned char r, unsigned char g, unsigned char b);
void v_drawPalette();
void v_waitVsync();

void v_drawRect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char color);
void v_fastFillRect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char color);
void v_memsetVGAASM(unsigned long offset, unsigned char color, unsigned int count);
void v_flipPage();

#pragma aux v_clearScreen =                                   \
    "mov dx, 0x3C4"        \
    "mov ax, 0x0F02"       \
    "out dx, ax"           \
    "mov eax, 0x00"        \
    "mov ecx, 16000"       \
    "mov edi, 0xA0000"     \
    "rep stosd"            \
    modify[eax ecx edi edx];

#pragma aux v_putPixelASM =                                  \
    "mov edi, 0xA0000"     \
    "add edi, eax"         \
    "mov [edi], dl"        \
    parm[eax][dl]          \
    modify[edi];

#pragma aux v_clearPageASM = \
    "mov edi, 0xA0000"     \
    "add edi, eax"         \
    "mov eax, ebx"         \
    "mov ecx, 16000"       \
    "rep stosb"            \
    parm [eax] [ebx]       \
    modify [edi eax ecx];

#pragma aux v_memsetVGAASM = \
    "mov edi, 0xA0000"     \
    "add edi, eax"         \
    "mov eax, ebx"         \
    "rep stosb"            \
    parm [eax] [ebx] [ecx] \
    modify [edi eax ecx];

#pragma aux v_setVideoMode13 = \
    "cld"                    \
    "mov ax, 0x0013",        \
    "int 0x10";
#pragma aux v_setTXTModeASM =    \
        "mov ax, 0x03",     \
        "int 0x10";

#pragma aux v_waitVsync = \
    "mov dx, 0x3DA"     \
    "v_wait1:"          \
    "in al, dx"         \
    "test al, 0x08"     \
    "jnz v_wait1"       \
    "v_wait2:"          \
    "in al, dx"         \
    "test al, 0x08"     \
    "jz v_wait2"        \
    modify [eax edx];



/* GLOBAL VARS =========================================================================*/

extern unsigned long pageOffsets[NUM_PAGES];

#define ENABLE_PAGE_FLIPPING 1

extern unsigned char currentPage;
extern unsigned char nextPage;

#endif