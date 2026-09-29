#ifndef PLATFORM_DOS_MODEX_H
#define PLATFORM_DOS_MODEX_H

#include "../../../hal/hal_modx.h"
#include "vgaregs.h"

/* Mode-X internal and ASM prototypes */
void _setVideoMode13(void);
void _set200pxMode(void);
void _clearScreen_asm(void);

void v_putPixelASM(unsigned long offset, unsigned char color);
void v_clearPageASM(unsigned long offset, unsigned char color);
void v_memsetVGAASM(
    unsigned long offset,
    unsigned char color,
    unsigned int count
);

/* Mode-X ASM Pragmas */
#pragma aux _setVideoMode13 = \
    "cld" \
    "mov ax, 0x0013" \
    "int 0x10";

#pragma aux _clearScreen_asm = \
    "mov dx, 0x3C4" \
    "mov ax, 0x0F02" \
    "out dx, ax" \
    "mov eax, 0x00" \
    "mov ecx, 16000" \
    "mov edi, 0xA0000" \
    "rep stosd" \
    modify[eax ecx edi edx];

#pragma aux v_putPixelASM = \
    "mov edi, 0xA0000" \
    "add edi, eax" \
    "mov [edi], dl" \
    parm[eax][dl] \
    modify[edi];

#pragma aux v_clearPageASM = \
    "mov edi, 0xA0000" \
    "add edi, eax" \
    "mov eax, ebx" \
    "mov ecx, 16000" \
    "rep stosb" \
    parm [eax] [ebx] \
    modify [edi eax ecx];

#pragma aux v_memsetVGAASM = \
    "mov edi, 0xA0000" \
    "add edi, eax" \
    "mov eax, ebx" \
    "rep stosb" \
    parm [eax] [ebx] [ecx] \
    modify [edi eax ecx];

#define hal_vid_putPixelASM  v_putPixelASM
#define hal_vid_memsetVGAASM v_memsetVGAASM

#endif
