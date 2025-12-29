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
    "mov dx, 0x3C4"        \
    "mov ax, 0x0F02"       \
    "out dx, ax"           \
    "mov eax, 0x00"        \
    "mov ecx, 16000"       \
    "mov edi, 0xA0000"     \
    "rep stosd"            \
    modify[eax ecx edi edx];

#pragma aux putPixelASM =                                  \
    "mov edi, 0xA0000"     \
    "add edi, eax"         \
    "mov [edi], dl"        \
    parm[eax][dl]          \
    modify[edi];

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

void drawRect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char color);
void fastFillRect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char color);
void memsetVGAASM(unsigned long offset, unsigned char color, unsigned int count);
void flipPage();

#define PAGE_SIZE 16000
#define NUM_PAGES 3

extern unsigned long pageOffsets[NUM_PAGES];

#define ENABLE_PAGE_FLIPPING 1

extern unsigned char currentPage;
extern unsigned char nextPage;

#endif