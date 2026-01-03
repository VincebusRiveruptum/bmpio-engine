#ifndef ENGINE_INPUT_H
#define ENGINE_INPUT_H

#include "STD.H"
#include <i86.h>

#define ESC_KEY 27
#define UP_KEY 72
#define DOWN_KEY 80
#define LEFT_KEY 75
#define RIGHT_KEY 77
#define SPACE_KEY 32
#define ENTER_KEY 13
#define W_KEY 87
#define A_KEY 65
#define S_KEY 83
#define D_KEY 68

extern char keyboardInputBuff[16];
extern char *keyboardInput;
extern char lastKey;

bool checkAppEnd();
unsigned char listenKeys();

#endif
