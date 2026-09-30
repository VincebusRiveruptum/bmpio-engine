#ifndef ENGINE_STD_H
#define ENGINE_STD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#if defined(__WATCOMC__) || defined(__MSDOS__) || defined(DOS)
#include <conio.h>
#include <dos.h>
#endif

#ifndef __cplusplus
typedef unsigned char bool;
#define true 1
#define false 0
#endif

#endif
