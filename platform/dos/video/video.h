#ifndef PLATFORM_DOS_VIDEO_H
#define PLATFORM_DOS_VIDEO_H

#include "../../../hal/hal_vid.h"

/* Low-level register/assembly operations */
void hal_vid_set25Lines();
void hal_vid_set43Lines();
void hal_vid_set50Lines();
void hal_vid_set80x60();
void hal_vid_set132x25();
void hal_vid_set132x43();
void hal_vid_set132x50();
void hal_vid_set132x60();

/* ASM Prototypes and pragmas */
#pragma aux _set80x25_asm = \
    "mov ax, 0x1202" \
    "mov bl, 0x30" \
    "int 0x10" \
    "mov ax, 0x0083" \
    "int 0x10" \
    "mov ax, 0x1114" \
    "mov bl, 0x00" \
    "int 0x10" \
    modify[ax bx cx dx]

#pragma aux _set80x43_asm = \
    "mov ax, 0x1201" \
    "mov bl, 0x30" \
    "int 0x10" \
    "mov ax, 0x0083" \
    "int 0x10" \
    "mov ax, 0x1112" \
    "mov bl, 0x00" \
    "int 0x10" \
    modify[ax bx cx dx]

#pragma aux _set80x50_asm = \
    "mov ax, 0x1202" \
    "mov bl, 0x30" \
    "int 0x10" \
    "mov ax, 0x0083" \
    "int 0x10" \
    "mov ax, 0x1112" \
    "mov bl, 0x00" \
    "int 0x10" \
    modify[ax bx cx dx]

#pragma aux _set80x60_asm = \
    "mov ax, 0x1202" \
    "mov bl, 0x30" \
    "int 0x10" \
    "mov ax, 0x0083" \
    "int 0x10" \
    "mov ax, 0x1112" \
    "mov bl, 0x00" \
    "int 0x10" \
    modify[ax bx cx dx]

#pragma aux _set132x25_asm = \
    "mov ax, 0x4F02" \
    "mov bx, 0x8109" \
    "int 0x10" \
    "mov ax, 0x1112" \
    "mov bl, 0x00" \
    "int 0x10" \
    modify [ax bx cx dx]

#pragma aux _set132x43_asm = \
    "mov ax, 0x4F02" \
    "mov bx, 0x810A" \
    "int 0x10" \
    "mov ax, 0x1112" \
    "mov bl, 0x00" \
    "int 0x10" \
    modify [ax bx cx dx]

#pragma aux _set132x50_asm = \
    "mov ax, 0x4F02" \
    "mov bx, 0x810B" \
    "int 0x10" \
    "mov ax, 0x1112" \
    "mov bl, 0x00" \
    "int 0x10" \
    modify [ax bx cx dx]

#pragma aux _set132x60_asm = \
    "mov ax, 0x4F02" \
    "mov bx, 0x810C" \
    "int 0x10" \
    "mov ax, 0x1112" \
    "mov bl, 0x00" \
    "int 0x10" \
    modify [ax bx cx dx]

// ASM VGA Graph ====================================================

#pragma aux hal_vid_clearScreen = \
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

#pragma aux _setVideoMode13 = \
    "cld" \
    "mov ax, 0x0013" \
    "int 0x10";

#pragma aux hal_vid_waitVsync = \
    "mov dx, 0x3DA" \
    "v_wait1:" \
    "in al, dx" \
    "test al, 0x08" \
    "jnz v_wait1" \
    "v_wait2:" \
    "in al, dx" \
    "test al, 0x08" \
    "jz v_wait2" \
    modify [eax edx];

// These could stay private...
//void _setVideoMode13(void);
//void _set200pxMode(void);

void hal_vid_putPixelASM(unsigned long offset, unsigned char color);

void hal_vid_memsetVGAASM(
    unsigned long offset,
    unsigned char color,
    unsigned int count
);

#endif
