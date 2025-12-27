#include "INPUT.H"

// This is for recording the keyboard input in a small buffer.
char keyboardInputBuff[16];

// Dynamic keyboard buffer for UI and inputs
char *keyboardInput = NULL;

// Last key pressed
char lastKey = 0;

// BIOS Data Area (BDA) addresses for 32-bit protected mode
#define BDA_SEG   0x40
#define KBD_HEAD  0x1A
#define KBD_TAIL  0x1C
#define KBD_START 0x1E
#define KBD_END   0x3E

#define ESC_KEY 27

unsigned char kbd_ascii, kbd_scan;

unsigned short *head_ptr = NULL;
unsigned short *tail_ptr = NULL;
unsigned short *start_ptr = NULL;
unsigned short *end_ptr = NULL;

void initInput(){
    unsigned short head, tail, start, end;

    head_ptr = (unsigned short*)MK_FP(BDA_SEG, KBD_HEAD);
    tail_ptr = (unsigned short*)MK_FP(BDA_SEG, KBD_TAIL);
    start_ptr = (unsigned short*)MK_FP(BDA_SEG, KBD_START);
    end_ptr = (unsigned short*)MK_FP(BDA_SEG, KBD_END);

    // Reset current frame's key
    kbd_ascii = 0;
    kbd_scan = 0;

    head = *head_ptr;
    
    // The buffer stores ASCII and Scan code as a 16-bit word at 0x40:head
    // In linear 32-bit: 0x400 + head
    if (head != tail) {
        unsigned char *buf = (unsigned char *)MK_FP(BDA_SEG, head);

        kbd_ascii = buf[0];
        kbd_scan  = buf[1];

        start = *(unsigned short *)MK_FP(BDA_SEG, KBD_START);
        end   = *(unsigned short *)MK_FP(BDA_SEG, KBD_END);

        head += 2;
        if (head >= end)
            head = start;

        *head_ptr = head;
    }
}

bool checkAppEnd(){
    // Use kbd_ascii for checking exit
    if(kbd_ascii == ESC_KEY) return true;
    return false;
}