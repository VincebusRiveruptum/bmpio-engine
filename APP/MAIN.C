
/*
    BMP, mode X and doubly linked list test by Vincebus Riveruptum
    2024.

    ASM functions based on santiago romero's vga tutorial 
    and converted them to 32 bit with CHATGPT.

    Compiled with WATCOM C 10.6
    wcl386 bmptest2.c

*/

#include "ASSETS.H"

// Main loop
int main(int argc, char *argv[]){

    // BMP list loading,,,
    BMPfile *firstFile = loadBMPfile("..\\assets\\sk256.bmp");
    unsigned long gameTicks = 0;
    unsigned long index = 0;

    Node *popped = NULL;

    addBMPtoList(&bmpList, firstFile->bmpData);
    //invertList(&bmpList);

    set200pxMode();

    setPalette(firstFile->bmpData->palette);

    while (!checkAppEnd()){
        initInput();

        fastFillRect(0,0,320,200,index);
        drawBitmapDistorted(&(getNodeByIndex(&bmpList, 1)->data), 55, 50, 15, index);

        if (index == 255){
            index = 0;
        }else
        {
            index++;
        }
        if(ENABLE_PAGE_FLIPPING == 1){
            flipPage(); 
        }
        gameTicks++;
    }

    setTXTMode();

    printf("\n96 Tears...");

    if(bmpList) freeList(&bmpList);
    if(globalPalette) free(globalPalette);
    
    return 0;
}