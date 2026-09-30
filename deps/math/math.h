#ifndef MATH_H
#define MATH_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "../deps/log/log.h"

/* CONSTANTS =============================================================== */


#define PI 3.14159265358979323846
#define DEG2RAD (PI / 180.0)

/* PROTOTYPES ============================================================== */

int m_round(float x);

/* GLOBAL VARS ============================================================= */

extern long m_sintable[360];
extern long m_costable[360];

#endif
