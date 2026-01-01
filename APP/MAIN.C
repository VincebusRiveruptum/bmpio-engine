
/*
    BMP, mode X and doubly linked list test by Vincebus Riveruptum
    2024.

    ASM functions based on santiago romero's vga tutorial 
    and converted them to 32 bit with CHATGPT.

    Compiled with WATCOM C 10.6
    wcl386 bmptest2.c
*/

#include "ENGINE.H"

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

int main(int argc, char *argv[]){
    char *assetsPath = NULL;
    char renamonPath[128] = {0};

    BMPfile *renamonFile = NULL;
    Animation *renamonJumping = NULL; 

    loadEnv();
    logger("ENV loaded!");
    /* DEBUGGING STUFF =============*/
    assetsPath = (char*)getEnv("ASSETS_PATH");
    sprintf(&renamonPath, "%s\\jump\\FRAME1.bmp", assetsPath);
    
    renamonFile = loadBMPfile(renamonPath);
    
    renamonJumping = createAnimation(createCoordinates(0, 0, 0)); 
    loadAnimationFrames(renamonJumping, renamonFrames);
    
    /* WHOLE LOOP!==========================*/
    
    initTrig(); // Log the static load
    set200pxMode();
    setPalette(renamonFile->bmpData->palette);

    while (!checkAppEnd()){
        //fillScreen(0);
        initInput();
        
        render2d(gameTicks);
        if(ENABLE_PAGE_FLIPPING == 1){
            flipPage(); 
        }
        gameTicks++;
    }
    
    setTXTMode();

    printf("\n96 Tears...");

    if(globalPalette) free(globalPalette);

    return 0;
}