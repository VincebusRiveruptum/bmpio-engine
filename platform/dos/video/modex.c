#include "modex.h"
#include "video.h"

/* GLOBALS ================================================================= */

unsigned char currentPage = 0;
unsigned char nextPage = 1;
unsigned long pageOffsets[NUM_PAGES];

/* FUNCTIONS =============================================================== */

void _set200pxMode(void)
{
    int i = 0;

    _setVideoMode13();

    outPortw(CRTC_ADDR, 0x0011);
    outPortw(SEQU_ADDR, 0x0604);
    outPortw(CRTC_ADDR, 0xE317);
    outPortw(CRTC_ADDR, 0x0014);
    outPortw(SEQU_ADDR, 0x0F02);

    for (i = 0; i < NUM_PAGES; i++) {
        pageOffsets[i] = (unsigned long)i * PAGE_SIZE;
    }

    hal_modx_clearScreen();
}

void hal_modx_init(void)
{
    _set200pxMode();
}

static void _setPage(unsigned char page)
{
    unsigned short start_addr = 0;

    start_addr = (unsigned short)pageOffsets[page];

    outPortw(CRTC_ADDR, (unsigned short)(0x0C | (start_addr & 0xFF00)));
    outPortw(
        CRTC_ADDR,
        (unsigned short)(0x0D | ((start_addr << 8) & 0xFF00))
    );
}

void hal_modx_flipPage(void)
{
    hal_vid_waitVsync();
    _setPage(nextPage);
    currentPage = nextPage;
    nextPage = (currentPage + 1) % NUM_PAGES;
}

void hal_modx_clearScreen(void)
{
    _clearScreen_asm();
}

void hal_modx_selectPlane(unsigned char plane)
{
    outPortb(SEQU_ADDR, 0x02);
    outPortb(SEQU_ADDR + 1, (unsigned char)(0x01 << plane));
}

void hal_modx_putPixel(int x, int y, char color)
{
    unsigned long offs = 0;

    hal_modx_selectPlane((unsigned char)(x & 3));

    if (ENABLE_PAGE_FLIPPING == 1) {
        offs = (unsigned long)((y << 6) + (y << 4) + (x >> 2)) +
            pageOffsets[nextPage];
    } else {
        offs = (unsigned long)((y << 6) + (y << 4) + (x >> 2));
    }

    v_putPixelASM(offs, (unsigned char)color);
}

unsigned char hal_modx_getPixel(int x, int y)
{
    (void)x;
    (void)y;
    return 0;
}

void hal_modx_drawRect(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    unsigned char color
) {
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned long offs = 0;

    for (j = y1; j < y2; j++) {
        for (i = x1; i < x2; i++) {
            if (ENABLE_PAGE_FLIPPING == 1) {
                offs = (unsigned long)((j << 6) + (j << 4) + (i >> 2)) +
                    pageOffsets[nextPage];
            } else {
                offs = (unsigned long)((j << 6) + (j << 4) + (i >> 2));
            }

            outPortb(SEQU_ADDR, 0x02);
            outPortb(SEQU_ADDR + 1, 0x0F);
            v_putPixelASM(offs, color);
        }
    }
}

void hal_modx_fastFillRect(
    unsigned int x1,
    unsigned int y1,
    unsigned int x2,
    unsigned int y2,
    unsigned char color
) {
    unsigned int y = 0;
    unsigned int width_pixels = 0;
    unsigned int width_bytes = 0;
    unsigned int start_x_byte = 0;
    unsigned long page_offs = 0;
    unsigned long row_offs = 0;

    width_pixels = x2 - x1;
    width_bytes = width_pixels >> 2;
    start_x_byte = x1 >> 2;
    page_offs = pageOffsets[nextPage];

    outPortb(SEQU_ADDR, 0x02);
    outPortb(SEQU_ADDR + 1, 0x0F);

    for (y = y1; y < y2; y++) {
        row_offs = page_offs + (y << 6) + (y << 4) + start_x_byte;
        v_memsetVGAASM(row_offs, color, width_bytes);
    }
}
