#ifndef TEST_H
#define TEST_H

#include "assets.h"
#include "game.h"

/* CONSTANTS =============================================================== */

/* PROTOTYPES ============================================================== */

bool t_initTests(void);
void t_createRenamon(void);
void t_skullBgTest(void);
void t_testFloor(void);
void t_testFloor2(void);
void t_cokeCanTest(void);

/* GLOBAL VARS ============================================================= */

extern struct Color *testPalette;

#endif
