#ifndef ENGINE_MATH_H
#define ENGINE_MATH_H

#include "STD.H"

#define PI 3.14159265358979323846
#define DEG2RAD (PI / 180.0)

int round(float x);
void initTrig();

extern long sintable[360];
extern long costable[360];
extern int trigInitialized;

#endif