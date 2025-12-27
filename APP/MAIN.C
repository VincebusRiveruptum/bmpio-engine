
/*
    BMP, mode X and doubly linked list test by Vincebus Riveruptum
    2024.

    ASM functions based on santiago romero's vga tutorial 
    and converted them to 32 bit with CHATGPT.

    Compiled with WATCOM C 10.6
    wcl386 bmptest2.c

*/

#include "ASSETS.H"

const char *renamonFrames[] = {
    "..\\assets\\jump\\FRAME1.bmp",
    "..\\assets\\jump\\FRAME2.bmp",
    "..\\assets\\jump\\FRAME3.bmp",
    "..\\assets\\jump\\FRAME4.bmp",
    "..\\assets\\jump\\FRAME5.bmp",
    "..\\assets\\jump\\FRAME6.bmp",
    "..\\assets\\jump\\FRAME7.bmp",
    "..\\assets\\jump\\FRAME8.bmp",
    "..\\assets\\jump\\FRAME9.bmp",
    "..\\assets\\jump\\FRAME10.bmp",
    "..\\assets\\jump\\FRAME11.bmp",
    "..\\assets\\jump\\FRAME12.bmp",
    "..\\assets\\jump\\FRAME13.bmp",
    "..\\assets\\jump\\FRAME14.bmp",
    "..\\assets\\jump\\FRAME15.bmp",
    NULL
};

// Main loop
int main(int argc, char *argv[]){

    // BMP list loading,,,
    BMPfile *firstFile = loadBMPfile("..\\assets\\sk256.bmp");
    BMPfile *renamonFile = loadBMPfile("..\\assets\\jump\\FRAME1.bmp");

    Animation *renamonJumping = createAnimation(); 
    Sprite *renamonStanding = createSprite();
    addBMPtoList(&bmpList, firstFile->bmpData);
    //invertList(&bmpList);

    
    loadAnimationFrames(renamonJumping, renamonFrames);
    addAnimationToTable(renamonJumping);   
    setPalette(renamonFile->bmpData->palette);


    loadSprite(renamonStanding, "..\\assets\\jump\\FRAME1.bmp", createCoordinates(100, 100, 0), 0);
    addSpriteToTable(renamonStanding);
    
    
    set200pxMode();
    //setPalette(firstFile->bmpData->palette);
    while (!checkAppEnd()){
        initInput();
        
        render2d(gameTicks);
        //TODO: Move this background rendering to a separate funciton inside "render2d" method
        
        /*
        fastFillRect(0,0,320,200,index);
        drawBitmapDistorted(&(getNodeByIndex(&bmpList, 1)->data), 55, 50, 15, index);
        
        if (index == 255){
            index = 0;
        }else
        {
            index++;
        }
        */
    
        // ======================================
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