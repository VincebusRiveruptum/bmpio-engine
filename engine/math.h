#ifndef ENGINE_MATH_H
#define ENGINE_MATH_H

#include "std.h"
#include "../deps/log/log.h"

/* CONSTANTS =============================================================== */


#define PI 3.14159265358979323846
#define DEG2RAD (PI / 180.0)

/* PROTOTYPES ============================================================== */

int m_round(float x);
void m_initTrig(void);

/* GLOBAL VARS ============================================================= */

extern long m_sintable[360];
extern long m_costable[360];
extern int m_trigInitialized;

#endif
