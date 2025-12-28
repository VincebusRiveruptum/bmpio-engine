/*
	This file handles bmp file operations
*/

#ifndef ENGINE_ASSETS_H
#define ENGINE_ASSETS_H

// STD IMPORTS
#include "STD.H"

// INTERNAL DEPENDENCES
#include "LOG/LOG.H"

// EXTERNAL DEPENDENCES
#include "../DEPS/DATA/DATA.H"
#include "../DEPS/ENV/ENV.H"

// PROTOTYPES
#include "PROTO.H"
#include "VIDEO.H"

// ENGINE
#include "MATH.H"
#include "INPUT.H"

// GLOBAL VARS & TYPES
#include "VARS.H"

extern unsigned long gameTicks;
extern unsigned long index;
extern SpriteTable *spriteTable;

#endif