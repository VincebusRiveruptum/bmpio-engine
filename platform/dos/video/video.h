#ifndef PLATFORM_DOS_VIDEO_H
#define PLATFORM_DOS_VIDEO_H

#include "../../../hal/hal_vid.h"
#include "modex.h"
#include "text.h"
#include "vgaregs.h"

/* Common ASM Prototypes */
void _waitVsync_asm(void);

/* Common ASM Pragmas */
#pragma aux _waitVsync_asm = \
    "mov dx, 0x3DA" \
    "v_wait1:" \
    "in al, dx" \
    "test al, 0x08" \
    "jnz v_wait1" \
    "v_wait2:" \
    "in al, dx" \
    "test al, 0x08" \
    "jz v_wait2" \
    modify [eax edx];

#endif
