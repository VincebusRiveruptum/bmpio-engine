#ifndef HAL_TXT_H
#define HAL_TXT_H

#include "std.h"

/* CONSTANTS =============================================================== */

#define HAL_TXT_BUFFER_SIZE 8192
#define HAL_VIDEO_BUFFER_SIZE 8192

/* GLOBALS ================================================================= */

extern unsigned short *textmemptr;      /* Video buffer pointer */
extern unsigned short *backbuffer;      /* Backup buffer */

extern unsigned char VIDEO_COLS;
extern unsigned char VIDEO_ROWS;

extern unsigned char currentCursorX;
extern unsigned char currentCursorY;

/* PROTOTYPES ============================================================== */

void hal_txt_init(void);
void hal_txt_close(void);

void hal_txt_clearBuffer(unsigned short *buffer);
void hal_txt_refresh(void);
void hal_txt_putCursor(unsigned char x, unsigned char y);

unsigned short hal_txt_getBufferSize(void);

void hal_txt_set25Lines(void);
void hal_txt_set43Lines(void);
void hal_txt_set50Lines(void);
void hal_txt_set80x60(void);
void hal_txt_set132x25(void);
void hal_txt_set132x43(void);
void hal_txt_set132x50(void);
void hal_txt_set132x60(void);

/* Backward compatibility aliases */
#define hal_vid_clearBuffer        hal_txt_clearBuffer
#define hal_vid_refresh            hal_txt_refresh
#define hal_vid_putCursor          hal_txt_putCursor
#define hal_vid_getVideoBufferSize hal_txt_getBufferSize

#define hal_vid_set25Lines         hal_txt_set25Lines
#define hal_vid_set43Lines         hal_txt_set43Lines
#define hal_vid_set50Lines         hal_txt_set50Lines
#define hal_vid_set80x60           hal_txt_set80x60
#define hal_vid_set132x25          hal_txt_set132x25
#define hal_vid_set132x43          hal_txt_set132x43
#define hal_vid_set132x50          hal_txt_set132x50
#define hal_vid_set132x60          hal_txt_set132x60

#endif
