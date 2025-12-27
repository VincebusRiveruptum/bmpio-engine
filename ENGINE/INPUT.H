#ifndef ENGINE_INPUT_H
#define ENGINE_INPUT_H

#include "STD.H"

extern char keyboardInputBuff[16];
extern char *keyboardInput;
extern char lastKey;

void initInput();
bool checkAppEnd();

#endif
