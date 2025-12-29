#include "video.h"

unsigned char currentPage = 0;
unsigned char nextPage = 1;

unsigned long pageOffsets[NUM_PAGES];

    
void setTXTMode(){
    setTXTModeASM();
};

void set200pxMode(){
    int i;
    setVideoMode13(); // Call the BIOS to set mode 13h

    // Set VGA registers (these remain the same as in Borland C)
    outPortw(CRTC_ADDR, 0x0011); // Unprotect CRTC registers
    outPortw(SEQU_ADDR, 0x0604);
    outPortw(CRTC_ADDR, 0xE317);
    outPortw(CRTC_ADDR, 0x0014);
    outPortw(SEQU_ADDR, 0x0F02);

    // Initialize page offsets
    for (i = 0; i < NUM_PAGES; i++) {
        pageOffsets[i] = (unsigned long)i * PAGE_SIZE;
    }

    clearScreen(); // Call the clear screen function
}

// Basic pixel plotting
void putPixelX(int x, int y, char color){
    unsigned long offs;

    // Set the VGA plane and calculate the offset
    outPortb(SEQU_ADDR, 0x02);
    outPortb(SEQU_ADDR + 1, 0x01 << (x & 3));

    if(ENABLE_PAGE_FLIPPING == 1){
        offs = (y << 6) + (y << 4) + (x >> 2) + pageOffsets[nextPage];
    }else{
        offs = (y << 6) + (y << 4) + (x >> 2);
    }
    
    putPixelASM(offs, color); // Place the pixel
}

// Page buffering functions
void setPage(unsigned char page) {
    unsigned short start_addr = (unsigned short)pageOffsets[page];

    // Standard VGA practice: Write Start Address High then Low
    // Register 0x0C: Start Address High, Register 0x0D: Start Address Low
    outPortw(CRTC_ADDR, (unsigned short)(0x0C | (start_addr & 0xFF00)));
    outPortw(CRTC_ADDR, (unsigned short)(0x0D | ((start_addr << 8) & 0xFF00)));
}


#pragma aux clearPage =    \
    "mov edi, 0xA0000" /* VGA memory segment for mode 13h */ \
    "add edi, eax"                                          \
    "mov eax, ebx"                                          \
    "mov ecx, 16000"    /* Zero-fill the register */          \
    "rep stosd"        /* Fill VGA memory with zeros */      \
    parm[eax][ebx]                                               \
    modify[eax ebx ecx edi];


#pragma aux fastFill = \
    

void flipPage() {
    waitVsync();
    setPage(nextPage);      // Show the page we just finished drawing
    currentPage = nextPage; // This is now the visible page
    nextPage = (currentPage + 1) % NUM_PAGES; // Target the next one for drawing
}

void setPal(char color, unsigned char r, unsigned char g, unsigned char b){
    outPortb(0x3c8, color);
    outPortb(0x3c9, r);
    outPortb(0x3c9, g);
    outPortb(0x3c9, b);
}

void fillScreen(unsigned char color){
    clearPage(pageOffsets[nextPage], color);
}

void drawRect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char color){
	int i = 0, j = 0;
    unsigned long offs;

	for(j = y1 ; j < y2 ; j++ ){
        for(i = x1; i < x2 ; i++){
            if(ENABLE_PAGE_FLIPPING == 1){
                offs = (j << 6) + (j << 4) + (i >> 2) + pageOffsets[nextPage];
            }else{
                offs = (j << 6) + (j << 4) + (i >> 2);
            }
            
            outPortb(SEQU_ADDR, 0x02);
            outPortb(SEQU_ADDR + 1, 0xF);
		    putPixelASM(offs, color); // Place the pixel
        }
	}
}

void fastFillRect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char color) {
    int y;
    unsigned long row_offs;
    unsigned int width_pixels = x2 - x1;
    unsigned int width_bytes = width_pixels >> 2;
    unsigned int start_x_byte = x1 >> 2;
    unsigned long page_offs = pageOffsets[nextPage];

    // Set Map Mask to all planes
    outPortb(SEQU_ADDR, 0x02);
    outPortb(SEQU_ADDR + 1, 0x0F);

    for (y = y1; y < y2; y++) {
        row_offs = page_offs + (y << 6) + (y << 4) + start_x_byte;
        // Use optimized pragma routine instead of incompatible _asm block
        memsetVGAASM(row_offs, color, width_bytes);
    }
}
