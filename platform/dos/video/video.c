#include "video.h"

/* GLOBALS ================================================================= */

int hal_vid_currentMode = HAL_VID_80X25;

/* FUNCTIONS =============================================================== */

void hal_vid_waitVsync(void)
{
    _waitVsync_asm();
}

void hal_vid_setPal(
    char color,
    unsigned char r,
    unsigned char g,
    unsigned char b
) {
    outPortb(0x3C8, (unsigned char)color);
    outPortb(0x3C9, r);
    outPortb(0x3C9, g);
    outPortb(0x3C9, b);
}

unsigned char hal_vid_setVideoMode(unsigned char mode, unsigned char show_msg)
{
    (void)show_msg;
    hal_vid_currentMode = (int)mode;

    switch (mode) {
        case HAL_VID_80X25:
            hal_txt_set25Lines();
            break;
        case HAL_VID_80X43:
            hal_txt_set43Lines();
            break;
        case HAL_VID_80X50:
            hal_txt_set50Lines();
            break;
        case HAL_VID_80X60:
            hal_txt_set80x60();
            break;
        case HAL_VID_132X25:
            hal_txt_set132x25();
            break;
        case HAL_VID_132X43:
            hal_txt_set132x43();
            break;
        case HAL_VID_132X50:
            hal_txt_set132x50();
            break;
        case HAL_VID_132X60:
            hal_txt_set132x60();
            break;
        case HAL_VID_MODE13:
            _setVideoMode13();
            break;
        case HAL_VID_MODEX:
        case HAL_VID_MODEY:
            hal_modx_init();
            break;
        default:
            hal_txt_set25Lines();
            break;
    }

    return mode;
}

void hal_vid_cycleVideoModes(void)
{
    hal_vid_currentMode++;
    if (hal_vid_currentMode > HAL_VID_MODEY) {
        hal_vid_currentMode = 0;
    }
    hal_vid_setVideoMode((unsigned char)hal_vid_currentMode, HAL_SHOW_MSG);
}
