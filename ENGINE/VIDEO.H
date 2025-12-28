#ifndef ENGINE_VIDEO_H
#define ENGINE_VIDEO_H

#include "STD.H"
#include "VGAREGS.H"

void clearScreen();
void setVideoMode13();
void set200pxMode(void);
void setTXTMode(void);
void putPixelX(int, int, char);
void putPixelASM(unsigned long offset, unsigned char color);
unsigned char getPixelX(int, int);
void setPal(char color, unsigned char r, unsigned char g, unsigned char b);
void drawPalette();
void waitVsync();

#pragma aux clearScreen =                                   \
    "mov eax, 0x00"    /* Zero-fill the register */          \
    "mov ecx, 16000"   /* 64000 bytes / 4 = 16000 dwords */  \
    "mov edi, 0xA0000" /* VGA memory segment for mode 13h */ \
    "rep stosd"        /* Fill VGA memory with zeros */      \
    modify[eax ecx edi];

#pragma aux putPixelASM =                                    \
    "mov edi, 0xA0000" /* Set edi to VGA memory segment */ \
    "add edi, eax"     /* Add the offset to edi */         \
    "mov al, dl"       /* Move color into al */            \
    "stosb"            /* Store byte at VGA memory */      \
    parm[eax][dl]      /* eax for offset, dl for color */  \
    modify[edi al];    /* Registers modified */

#pragma aux clearPageASM = \
    "mov edi, 0xA0000"     \
    "add edi, eax"         \
    "mov eax, ebx"         \
    "mov ecx, 16000"       \
    "rep stosb"            \
    parm [eax] [ebx]       \
    modify [edi eax ecx];

#pragma aux memsetVGAASM = \
    "mov edi, 0xA0000"     \
    "add edi, eax"         \
    "mov eax, ebx"         \
    "rep stosb"            \
    parm [eax] [ebx] [ecx] \
    modify [edi eax ecx];

#pragma aux setVideoMode13 = \
    "cld"                    \
    "mov ax, 0x0013",        \
    "int 0x10";
#pragma aux setTXTModeASM =    \
        "mov ax, 0x03",     \
        "int 0x10";

#pragma aux waitVsync = \
    "mov dx, 3dah"      \
    "wait_end:"         \
    "in al, dx"         \
    "test al, 08h"      \
    "jnz wait_end"      \
    "wait_start:"       \
    "in al, dx"         \
    "test al, 08h"      \
    "jz wait_start"     \
    modify [eax edx];

void drawRect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char color);
void fastFillRect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char color);
void memsetVGAASM(unsigned long offset, unsigned char color, unsigned int count);
void flipPage();

#define PAGE_SIZE 16000
#define NUM_PAGES 4

extern unsigned long pageOffsets[NUM_PAGES];

#define ENABLE_PAGE_FLIPPING 1

extern unsigned char currentPage;
extern unsigned char nextPage;

#endif