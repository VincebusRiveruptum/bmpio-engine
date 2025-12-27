#include "video.h"

unsigned char currentPage = 0;
unsigned char nextPage = 1;

    
void setTXTMode(){
    setTXTModeASM();
};

// Setting mode x
void set200pxMode(){
    setVideoMode13(); // Call the BIOS to set mode 13h

    // Set VGA registers (these remain the same as in Borland C)
    outPortw(SEQU_ADDR, 0x0604);
    outPortw(CRTC_ADDR, 0xE317);
    outPortw(CRTC_ADDR, 0x0014);
    outPortw(SEQU_ADDR, 0x0F02);

    clearScreen(); // Call the clear screen function
}

#pragma aux putPixelASM =                                    \
    "mov edi, 0xA0000" /* Set edi to VGA memory segment */ \
    "add edi, eax"     /* Add the offset to edi */         \
    "mov al, dl"       /* Move color into al */            \
    "stosb"            /* Store byte at VGA memory */      \
    parm[eax][dl]      /* eax for offset, dl for color */  \
    modify[edi al];    /* Registers modified */

void putPixelX(int x, int y, char color){
    unsigned long offs;

    // Set the VGA plane and calculate the offset
    outPortb(SEQU_ADDR, 0x02);
    outPortb(SEQU_ADDR + 1, 0x01 << (x & 3));

    if(ENABLE_PAGE_FLIPPING == 1){
        offs = (y << 6) + (y << 4) + (x >> 2) + (PAGE_SIZE * nextPage); // Equivalent to y*320 + x/4 in mode 13h
    }else{
        offs = (y << 6) + (y << 4) + (x >> 2); // Equivalent to y*320 + x/4 in mode 13h
    }
    
    putPixelASM(offs, color); // Place the pixel
}

// Page buffering functions
void setPage(unsigned char page) {
    unsigned long start_addr = (page * PAGE_SIZE);

    outPortb(CRTC_ADDR, 0x0C);        // CRTC register 0x0C (high byte of start address)
    outPortb(CRTC_ADDR + 1, (start_addr >> 8));
    outPortb(CRTC_ADDR, 0x0D);        // CRTC register 0x0D (low byte of start address)
    outPortb(CRTC_ADDR + 1, start_addr & 0xFF);
}

#pragma aux clearPage =    \
    "imul eax, 16000"              /* Multiply page size with page number*/\
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
    currentPage = nextPage;
    setPage(currentPage); // Perform the page flip
    nextPage = (nextPage + 1) % NUM_PAGES;
}

void setPal(char color, unsigned char r, unsigned char g, unsigned char b){
    outPortb(0x3c8, color);
    outPortb(0x3c9, r);
    outPortb(0x3c9, g);
    outPortb(0x3c9, b);
}

void fillScreen(unsigned char color){
    clearPage(nextPage, color);
}

void drawRect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned char color){
	int i = 0, j = 0;
    unsigned long offs;

	for(j = y1 ; j < y2 ; j++ ){
        for(i = x1; i < x2 ; i++){
            if(ENABLE_PAGE_FLIPPING == 1){
                offs = (j << 6) + (j << 4) + (i >> 2) + (PAGE_SIZE * nextPage); // Equivalent to y*320 + x/4 in mode 13h

            }else{
                offs = (j << 6) + (j << 4) + (i >> 2); // Equivalent to y*320 + x/4 in mode 13h
            }
            
            outPortb(SEQU_ADDR, 0x02);
            outPortb(SEQU_ADDR + 1, 0xF);
		    putPixelASM(offs, color); // Place the pixel
        }
	}
}
