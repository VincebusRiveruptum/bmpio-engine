#ifndef HAL_VID_H
#define HAL_VID_H

#include "std.h"
#include "hal_modx.h"
#include "hal_txt.h"

/* CONSTANTS =============================================================== */

#define HAL_SHOW_MSG 0
#define HAL_NO_MSG   1

/* TYPES =================================================================== */

typedef enum VideoModeType {
    HAL_VID_TEXT,
    HAL_VID_GRAPH
} VideoModeType;

typedef enum VideoMode {
    /* Text */
    HAL_VID_80X25,
    HAL_VID_80X43,
    HAL_VID_80X50,
    HAL_VID_80X60,
    HAL_VID_132X25,
    HAL_VID_132X43,
    HAL_VID_132X50,
    HAL_VID_132X60,

    /* Graphics */
    HAL_VID_MODE13,
    HAL_VID_MODEX,
    HAL_VID_MODEY
} VideoMode;

/* GLOBALS ================================================================= */

extern int hal_vid_currentMode;

/* PROTOTYPES ============================================================== */

unsigned char hal_vid_setVideoMode(unsigned char mode, unsigned char show_msg);
void hal_vid_cycleVideoModes(void);

void hal_vid_waitVsync(void);
void hal_vid_setPal(
    char color,
    unsigned char r,
    unsigned char g,
    unsigned char b
);

/* Backward compatibility aliases */
#define v_waitVsync      hal_vid_waitVsync
#define v_setPal         hal_vid_setPal
#define v_set200pxMode() hal_vid_setVideoMode(HAL_VID_MODEX, HAL_NO_MSG)
#define v_setTXTMode()   hal_vid_setVideoMode(HAL_VID_80X25, HAL_NO_MSG)

#endif
