
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
    char *assetsPath = NULL;
    
    char sk256Path[128] = {0};
    char renamonPath[128] = {0};
    char *bgSkullsFrames[2] = {NULL, NULL};
    char *logType = NULL;
    
    BMPfile *bgSkullsFile = NULL;
    BMPfile *renamonFile = NULL;

    Animation *renamonJumping = NULL; 
    Sprite *renamonStanding = NULL;
    Animation *bgSkullsAnimation = NULL;

    RotationTransformation bgSkullsRot;
    Transformation bgSkullsTransformation;
    
    bgSkullsRot.angle = 1;      // Speed: 1 degree per frame
    bgSkullsRot.current = 0;    // Starting angle

    bgSkullsTransformation.type = TR_ROTATION;
    bgSkullsTransformation.data = &bgSkullsRot;

    loadEnv();
    logger("ENV loaded!");
    
    initTrig(); // Log the static load
    
    /* DEBUGGING STUFF =============*/
    
    assetsPath = (char*)getEnv("ASSETS_PATH");
    
    sprintf(&sk256Path, "%s\\sk256.bmp", assetsPath);
    sprintf(&renamonPath, "%s\\jump\\FRAME1.bmp", assetsPath);
    
    bgSkullsFile = loadBMPfile(sk256Path);
    renamonFile = loadBMPfile(renamonPath);

    renamonJumping = createAnimation(createCoordinates(100, 100, 0)); 
    renamonStanding = createSprite();
    bgSkullsAnimation = createAnimation(createCoordinates(50, 50, 0));
    
    bgSkullsFrames[0] = sk256Path;
    bgSkullsFrames[1] = NULL;

    loadAnimationFrames(bgSkullsAnimation, bgSkullsFrames);
    addAnimationToTable(bgSkullsAnimation);
    addTransformation(bgSkullsAnimation, &bgSkullsTransformation);
    
    loadAnimationFrames(renamonJumping, renamonFrames);
    addAnimationToTable(renamonJumping);   
    
    loadSprite(renamonStanding, "..\\assets\\jump\\FRAME1.bmp", createCoordinates(200, 100, 0), 0);
    addSpriteToTable(renamonStanding);
    
    set200pxMode();
    setPalette(renamonFile->bmpData->palette);
    
    /* WHOLE LOOP!==========================*/
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

    if(bmpList) freeList(&bmpList);
    if(globalPalette) free(globalPalette);

    logger("What did you expect?");

    return 0;
}