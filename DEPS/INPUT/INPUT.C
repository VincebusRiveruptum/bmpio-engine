#include "INPUT.H"

// This is for recording the keyboard input in a small buffer.
char keyboardInputBuff[16];

// Dynamic keyboard buffer for UI and inputs
char *keyboardInput = NULL;

// Last key pressed
char lastKey = 0;

// BIOS Data Area (BDA) addresses for 32-bit protected mode
extern int keyAvailableASM(void);
#pragma aux keyAvailableASM = \
    "mov ah, 01h"             \
    "int 16h"                 \
    "mov eax, 0"              \
    "jz no_key"               \
    "inc eax"                 \
    "no_key:"                 \
    value [eax]               \
    modify [ah];

static int keyAvailable(void)
{
    return keyAvailableASM();
}

extern unsigned short readKeyASM(void);
#pragma aux readKeyASM = \
    "mov ah, 00h"         \
    "int 16h"             \
    value [ax]            \
    modify [ax];

static unsigned short readKey(void)
{
    return readKeyASM();
}

unsigned char kbd_ascii, kbd_scan;

bool checkAppEnd(){
    if(keyAvailable()){
        kbd_ascii = readKey() & 0xFF;
        if(kbd_ascii == ESC_KEY) return true;
    }
    return false;
}

unsigned char listenKeys(){
    if(keyAvailable()){
        kbd_ascii = readKey() & 0xFF;
        return kbd_ascii;
    }
    return 0;
}