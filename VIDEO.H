#ifndef VIDEO_H
#define VIDEO_H

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include "vgaregs.h"

void clearScreen();
void setVideoMode13();
void set200pxMode(void);
void setTXTMode(void);
void putPixelX(int, int, char);
unsigned char getPixelX(int, int);
void setPal(char color, unsigned char r, unsigned char g, unsigned char b);
void drawPalette();

#pragma aux clearScreen =                                   \
    "mov eax, 0x00"    /* Zero-fill the register */          \
    "mov ecx, 16000"   /* 64000 bytes / 4 = 16000 dwords */  \
    "mov edi, 0xA0000" /* VGA memory segment for mode 13h */ \
    "rep stosd"        /* Fill VGA memory with zeros */      \
    modify[eax ecx edi];

#pragma aux setVideoMode13 = \
    "cld"                    \
    "mov ax, 0x0013",        \
    "int 0x10";

#pragma aux setTXTModeASM =    \
        "xor ax, ax",       \ 
        "int 0x16",         \
        "mov ax, 0x03",     \
        "int 0x10";

#define PAGE_SIZE 16000
#define NUM_PAGES 4

#define ENABLE_PAGE_FLIPPING 1

extern unsigned char currentPage = 0;
extern unsigned char nextPage = 1;

#endif